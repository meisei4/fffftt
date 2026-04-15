#include "fffftt.h"
#include "../sh4zam/include/sh4zam/shz_sh4zam.h"
#include "raylib.h"
#include "rlgl.h"
#include <GL/gl.h>

static const char* domain = "FFT-TERRAIN-3D-DC";

#define LANE_COUNT 24
#define LANE_POINT_COUNT 128

#define TERRAIN_MESH_VERTEX_COUNT (LANE_COUNT * LANE_POINT_COUNT)
#define TERRAIN_LINE_SEGMENT_COUNT (LANE_COUNT * (LANE_POINT_COUNT - 1))
#define TERRAIN_LINE_INDEX_COUNT (TERRAIN_LINE_SEGMENT_COUNT * 2)

#define TERRAIN_TRIANGLE_COUNT ((LANE_COUNT - 1) * (LANE_POINT_COUNT - 1)) * 2
#define TERRAIN_TRIANGLE_INDEX_COUNT (TERRAIN_TRIANGLE_COUNT * 3)

#define AMPLITUDE_Y_SCALE 2.0f
#define TERRAIN_HALF_SPAN 0.5f
#define TERRAIN_LINE_WIDTH_RASTER_PIXELS 2.0f
#define TERRAIN_POINT_SIZE_RASTER_PIXELS 3.0f
#define LINE_LENGTH_SCALE 4.0f
#define LANE_SPACING_SCALE 0.25f

static Mesh mesh = {0};
static float lane_point_samples[LANE_COUNT][LANE_POINT_COUNT] = {0};
static float fft_window_samples[BUFFER_SIZE] = {0};

static void advance_fft_history(void);
static void update_mesh_vertices(void);
static void update_camera_orbit(Camera3D* camera, float dt);
static void fill_mesh_colors(void);
static void fill_mesh_indices_lane_topology(void);

static unsigned short* scratch_triangle_indices = NULL;

static Color* scratch_overlay_line_colors = NULL;
static unsigned short* scratch_overlay_point_indices = NULL;
static Color* scratch_overlay_point_colors = NULL;

int main(void) {
    FFTData fft_data = {0};
    float fft_compute_ms = 0.0f;
    float audio_samples[WINDOW_SIZE] = {0};
    int16_t chunk_samples[AUDIO_STREAM_RING_BUFFER_SIZE] = {0};

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    float start_time = (float)GetTime();
    FFT_PROFILE_DEFINE(fft_profile_data);
    fft_data.tapback_pos = TAPBACK_POS_DEFAULT;
    fft_data.work_buffer = RL_CALLOC(WINDOW_SIZE, sizeof(FFTComplex));
    fft_data.prev_magnitudes = RL_CALLOC(BUFFER_SIZE, sizeof(float));
    fft_data.fft_history = RL_CALLOC(FFT_HISTORY_FRAME_COUNT, sizeof(float[BUFFER_SIZE]));

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_STREAM_RING_BUFFER_SIZE);
    Wave wav = LoadWave(RD_SHADERTOY_EXPERIMENT_22K_WAV);
    WaveFormat(&wav, SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    AudioStream audio_stream = LoadAudioStream(SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    PlayAudioStream(audio_stream);

    size_t wav_cursor = 0;
    int16_t* wav_pcm16 = (int16_t*)wav.data;

    Camera3D camera = {
        .position =
            (Vector3){-1.36625f * 2.0f, 1.345f * 2.0f, 1.45625f * 2.0f}, //TODO: just multiply by 2 to move the camera away to avoid clipping in orthographic...
        .target = (Vector3){0.0f, 0.25f, 0.0f},
        .up = (Vector3){0.0f, 1.0f, 0.0f},
        .fovy = 3.111f,
        .projection = CAMERA_ORTHOGRAPHIC,
    };

    mesh = (Mesh){
        .vertexCount = TERRAIN_MESH_VERTEX_COUNT,
        .triangleCount = TERRAIN_LINE_SEGMENT_COUNT,
        .vertices = RL_CALLOC(TERRAIN_MESH_VERTEX_COUNT * 3, sizeof(float)),
        .colors = RL_CALLOC(TERRAIN_MESH_VERTEX_COUNT, sizeof(Color)),
        .indices = RL_CALLOC(TERRAIN_LINE_INDEX_COUNT, sizeof(unsigned short)),
    };

    update_mesh_vertices();
    fill_mesh_indices_lane_topology();
    fill_mesh_colors();

    scratch_triangle_indices = RL_CALLOC(TERRAIN_TRIANGLE_INDEX_COUNT, sizeof(unsigned short));
    for (int i = 0, k = 0; i < LANE_COUNT - 1; i++) {
        for (int j = 0; j < LANE_POINT_COUNT - 1; j++) {
            unsigned short a = (unsigned short)(i * LANE_POINT_COUNT + j);
            scratch_triangle_indices[k++] = a;
            scratch_triangle_indices[k++] = a + 1;
            scratch_triangle_indices[k++] = a + LANE_POINT_COUNT;
            scratch_triangle_indices[k++] = a + 1;
            scratch_triangle_indices[k++] = a + LANE_POINT_COUNT + 1;
            scratch_triangle_indices[k++] = a + LANE_POINT_COUNT;
        }
    }

    scratch_overlay_line_colors = RL_CALLOC(mesh.vertexCount, sizeof(Color));
    scratch_overlay_point_indices = RL_CALLOC(mesh.vertexCount, sizeof(unsigned short));
    scratch_overlay_point_colors = RL_CALLOC(mesh.vertexCount, sizeof(Color));
    for (int i = 0; i < mesh.vertexCount; i++) {
        scratch_overlay_line_colors[i] = BLUE;
        scratch_overlay_point_indices[i] = (unsigned short)i;
        scratch_overlay_point_colors[i] = MAGENTA;
    }

    Model model = LoadModelFromMesh(mesh);
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
    Mesh* draw_mesh = &model.meshes[0];

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        while (IsAudioStreamProcessed(audio_stream)) {
            for (int i = 0; i < AUDIO_STREAM_RING_BUFFER_SIZE; i++) {
                chunk_samples[i] = wav_pcm16[wav_cursor];
                if (++wav_cursor >= wav.frameCount) {
                    wav_cursor = 0;
                }
            }

            UpdateAudioStream(audio_stream, chunk_samples, AUDIO_STREAM_RING_BUFFER_SIZE);

            for (int i = 0; i < WINDOW_SIZE; i++) {
                audio_samples[i] = (float)chunk_samples[WINDOW_SIZE + i] / PCM_SAMPLE_MAX_F;
            }

            apply_blackman_window(&fft_data, audio_samples);
            uint64_t fft_start = time_nanoseconds();
            shz_fft((shz_complex_t*)fft_data.work_buffer, (size_t)WINDOW_SIZE);
            fft_compute_ms = elapsed_milliseconds(fft_start);

            clean_up_fft(&fft_data);

            FFT_PROFILE_SAMPLE(fft_profile_data, domain, fft_compute_ms, (float)GetTime() - start_time, &fft_data);

            float* fft_frame = fft_data.fft_history[(fft_data.history_pos - 1 + FFT_HISTORY_FRAME_COUNT) % FFT_HISTORY_FRAME_COUNT];
            for (int i = 0; i < BUFFER_SIZE; i++) {
                fft_window_samples[i] = fft_frame[i];
            }

            advance_fft_history();
        }

        update_mesh_vertices();
        update_camera_orbit(&camera, GetFrameTime());

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);
        unsigned short* saved_indices = (unsigned short*)draw_mesh->indices;
        int saved_triangle_count = draw_mesh->triangleCount;
        unsigned int saved_texture_id = model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id;
        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = 0;

        draw_mesh->indices = scratch_triangle_indices;
        draw_mesh->triangleCount = TERRAIN_TRIANGLE_COUNT;
        // DrawModelEx(model, (Vector3){0.0f}, (Vector3){0.0f, 1.0f, 0.0f}, 0.0f, (Vector3){1.0f, 1.0f, 1.0f}, WHITE);

        draw_mesh->indices = saved_indices;
        draw_mesh->triangleCount = saved_triangle_count;
        unsigned char* saved_colors = draw_mesh->colors;
        // draw_mesh->colors = (unsigned char*)scratch_overlay_line_colors;
        rlSetLineWidth(TERRAIN_LINE_WIDTH_RASTER_PIXELS);
        DrawModelWiresEx(model, (Vector3){0.0f}, (Vector3){0.0f, 1.0f, 0.0f}, 0.0f, (Vector3){1.0f, 1.0f, 1.0f}, WHITE);

        rlSetPointSize(TERRAIN_POINT_SIZE_RASTER_PIXELS);
        draw_mesh->indices = scratch_overlay_point_indices;
        // draw_mesh->colors = (unsigned char*)scratch_overlay_point_colors;
        // DrawModelPointsEx(model, (Vector3){0.0f}, (Vector3){0.0f, 1.0f, 0.0f}, 0.0f, (Vector3){1.0f, 1.0f, 1.0f}, WHITE);

        draw_mesh->indices = saved_indices;
        // draw_mesh->colors = saved_colors;
        draw_mesh->triangleCount = saved_triangle_count;
        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = saved_texture_id;

        EndMode3D();
        DrawFPS(540, 420);
        EndDrawing();
    }

    UnloadModel(model);
    UnloadAudioStream(audio_stream);
    UnloadWave(wav);
    CloseAudioDevice();
    RL_FREE(fft_data.fft_history);
    RL_FREE(fft_data.prev_magnitudes);
    RL_FREE(fft_data.work_buffer);
    CloseWindow();
    return 0;
}

static void advance_fft_history(void) {
    for (int i = LANE_COUNT - 1; i > 0; i--) {
        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            lane_point_samples[i][j] = lane_point_samples[i - 1][j];
        }
    }

    for (int i = 0; i < LANE_POINT_COUNT; i++) {
        lane_point_samples[0][i] = fft_window_samples[(i * (BUFFER_SIZE - 1)) / (LANE_POINT_COUNT - 1)];
    }
}

static void update_mesh_vertices(void) {
    for (int i = 0; i < LANE_COUNT; i++) {
        float lane_offset = ((float)i - 0.5f * (float)(LANE_COUNT - 1)) * LANE_SPACING_SCALE;
        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            int vertex_offset = (i * LANE_POINT_COUNT + j) * 3;
            float x = (((float)j / (float)(LANE_POINT_COUNT - 1)) - TERRAIN_HALF_SPAN) * LINE_LENGTH_SCALE;
            float y = lane_point_samples[i][j] * AMPLITUDE_Y_SCALE;
            float z = -lane_offset;
            mesh.vertices[vertex_offset + 0] = x;
            mesh.vertices[vertex_offset + 1] = y;
            mesh.vertices[vertex_offset + 2] = z;
        }
    }
}

static void update_camera_orbit(Camera3D* camera, float dt) {
    Vector3 dist_from_target = Vector3Subtract(camera->position, camera->target);
    float orbit_radius = Vector3Length(dist_from_target);
    float yaw = atan2f(dist_from_target.z, dist_from_target.x);
    float ground_radius = sqrtf(dist_from_target.x * dist_from_target.x + dist_from_target.z * dist_from_target.z);
    float pitch = atan2f(dist_from_target.y, ground_radius);
    float fovy = camera->fovy;
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT))
        yaw += CAMERA_ORBIT_VELOCITY * dt;
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT))
        yaw -= CAMERA_ORBIT_VELOCITY * dt;
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP))
        pitch += CAMERA_ORBIT_VELOCITY * dt;
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN))
        pitch -= CAMERA_ORBIT_VELOCITY * dt;
    if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) > 0.0f)
        fovy -= GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) * CAMERA_FOVY_VELOCITY * dt;
    if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_TRIGGER) > 0.0f)
        fovy += GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_TRIGGER) * CAMERA_FOVY_VELOCITY * dt;

    pitch = Clamp(pitch, CAMERA_PITCH_MIN, CAMERA_PITCH_MAX);
    fovy = Clamp(fovy, CAMERA_FOVY_MIN, CAMERA_FOVY_MAX);
    camera->position.x = camera->target.x + orbit_radius * cosf(pitch) * cosf(yaw);
    camera->position.y = camera->target.y + orbit_radius * sinf(pitch);
    camera->position.z = camera->target.z + orbit_radius * cosf(pitch) * sinf(yaw);
    camera->fovy = fovy;
}

static void fill_mesh_colors(void) {
    Color* colors = (Color*)mesh.colors;
    for (int i = 0; i < LANE_COUNT; i++) {
        float v = (float)i / (float)(LANE_COUNT - 1);
        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            float u = (float)j / (float)(LANE_POINT_COUNT - 1);
            int vertex_index = i * LANE_POINT_COUNT + j;
            float r = (1.0f - u) * (1.0f - v) * MAGENTA.r + u * (1.0f - v) * BLUE.r + (1.0f - u) * v * RED.r + u * v * YELLOW.r;
            float g = (1.0f - u) * (1.0f - v) * MAGENTA.g + u * (1.0f - v) * BLUE.g + (1.0f - u) * v * RED.g + u * v * YELLOW.g;
            float b = (1.0f - u) * (1.0f - v) * MAGENTA.b + u * (1.0f - v) * BLUE.b + (1.0f - u) * v * RED.b + u * v * YELLOW.b;
            colors[vertex_index] = (Color){(unsigned char)r, (unsigned char)g, (unsigned char)b, 255};
        }
    }
}

static void fill_mesh_indices_lane_topology(void) {
    unsigned short* indices = mesh.indices;
    int index_cursor = 0;
    for (int i = 0; i < LANE_COUNT; i++) {
        for (int j = 0; j < LANE_POINT_COUNT - 1; j++) {
            unsigned short i_0 = (unsigned short)(i * LANE_POINT_COUNT + j);
            unsigned short i_1 = (unsigned short)(i_0 + 1);
            indices[index_cursor++] = i_0;
            indices[index_cursor++] = i_1;
        }
    }
}
