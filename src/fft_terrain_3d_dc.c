#include "fffftt.h"
#include "../sh4zam/include/sh4zam/shz_sh4zam.h"
#include "raylib.h"
#include "rlgl.h"
#include <GL/gl.h>

#define LINE_WIDTH_RASTER_PIXELS 2.0f
#define POINT_SIZE_RASTER_PIXELS 3.0f

static const char* domain = "FFT-TERRAIN-3D-DC";

static Mesh mesh = {0};
static float lane_point_samples[LANE_COUNT][LANE_POINT_COUNT] = {0};
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
        .vertexCount = MESH_VERTEX_COUNT,
        .triangleCount = LINE_SEGMENT_COUNT,
        .vertices = RL_CALLOC(MESH_VERTEX_COUNT * 3, sizeof(float)),
        .colors = RL_CALLOC(MESH_VERTEX_COUNT, sizeof(Color)),
        .indices = RL_CALLOC(LINE_INDEX_COUNT, sizeof(unsigned short)),
    };

    update_terrain_mesh_vertices(mesh.vertices, &lane_point_samples[0][0]);
    fill_mesh_indices_lane_topology(mesh.indices);
    fill_mesh_colors((Color*)mesh.colors);

    scratch_triangle_indices = RL_CALLOC(TRIANGLE_INDEX_COUNT, sizeof(unsigned short));
    fill_mesh_indices_triangle_topology(scratch_triangle_indices);

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
            advance_lane_history(&lane_point_samples[0][0]);
            for (int i = 0; i < LANE_POINT_COUNT; i++) {
                lane_point_samples[0][i] = fft_frame[(i * (BUFFER_SIZE - 1)) / (LANE_POINT_COUNT - 1)];
            }
        }

        update_terrain_mesh_vertices(mesh.vertices, &lane_point_samples[0][0]);
        update_camera_orbit(&camera, GetFrameTime());

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);
        unsigned short* saved_indices = (unsigned short*)draw_mesh->indices;
        int saved_triangle_count = draw_mesh->triangleCount;
        unsigned int saved_texture_id = model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id;
        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = 0;

        draw_mesh->indices = scratch_triangle_indices;
        draw_mesh->triangleCount = TRIANGLE_COUNT;
        // DrawModelEx(model, (Vector3){0.0f}, (Vector3){0.0f, 1.0f, 0.0f}, 0.0f, (Vector3){1.0f, 1.0f, 1.0f}, WHITE);

        draw_mesh->indices = saved_indices;
        draw_mesh->triangleCount = saved_triangle_count;
        unsigned char* saved_colors = draw_mesh->colors;
        // draw_mesh->colors = (unsigned char*)scratch_overlay_line_colors;
        rlSetLineWidth(LINE_WIDTH_RASTER_PIXELS);
        DrawModelWiresEx(model, (Vector3){0.0f}, (Vector3){0.0f, 1.0f, 0.0f}, 0.0f, (Vector3){1.0f, 1.0f, 1.0f}, WHITE);

        rlSetPointSize(POINT_SIZE_RASTER_PIXELS);
        draw_mesh->indices = scratch_overlay_point_indices;
        // draw_mesh->colors = (unsigned char*)scratch_overlay_point_colors;
        // DrawModelPointsEx(model, (Vector3){0.0f}, (Vector3){0.0f, 1.0f, 0.0f}, 0.0f, (Vector3){1.0f, 1.0f, 1.0f}, WHITE);

        draw_mesh->indices = saved_indices;
        // draw_mesh->colors = saved_colors;
        draw_mesh->triangleCount = saved_triangle_count;
        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = saved_texture_id;

        EndMode3D();
        // DrawFPS(540, 420);
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
