#include "fffftt.h"
#include "../sh4zam/include/sh4zam/shz_sh4zam.h"
#include "raylib.h"
#include "rlgl.h"
#include <GL/gl.h>

#define LINE_WIDTH_RASTER_PIXELS 2.0f
#define POINT_SIZE_RASTER_PIXELS 3.0f
#define ONSET_LAG_FRAMES 1
#define ONSET_ENVELOPE_RADIUS 2
#define ONSET_STRENGTH_MIN 0.0f
#define ONSET_STRENGTH_MAX 0.025f
#define ONSET_ATTACK_RATE 0.95f
#define ONSET_DECAY_RATE 0.10f
#define LIGHT0_DIFFUSE_MIN 0.0f
#define LIGHT0_DIFFUSE_MAX 1.5f

static const char* domain = "FFT-TERRAIN-3D-DC";

static Mesh mesh_a = {0};
static Mesh mesh_b = {0};
static Mesh flat_mesh = {0};

static Model model_a = {0};
static Model model_b = {0};
static Model flat_model = {0};

static void update_onset_interpolation_factor(FFTData* fft_data);
static void update_light_constants(void);

static float lane_point_values[LANE_COUNT][LANE_POINT_COUNT] = {0};
static float analysis_window_samples[ANALYSIS_WINDOW_SIZE_IN_FRAMES] = {0};
static float onset_interpolation_factor = 0.0f;
static GLfloat light0_diffuse[4] = {LIGHT0_DIFFUSE_MIN, LIGHT0_DIFFUSE_MIN, LIGHT0_DIFFUSE_MIN, 1.0f};

static float vertices[MESH_VERTEX_COUNT * 3] = {0};
static float normals[MESH_VERTEX_COUNT * 3] = {0};
static Color colors[MESH_VERTEX_COUNT] = {0};

static float flat_vertices[FLAT_VERTEX_COUNT * 3] = {0};
static float flat_normals[FLAT_VERTEX_COUNT * 3] = {0};
static Color flat_colors[FLAT_VERTEX_COUNT] = {0};

int main(void) {
    FFTData fft_data = {0};
    int16_t chunk_samples[AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES] = {0};

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    fft_data.tapback_pos = ANALYSIS_TAPBACK_POS_DEFAULT;
    fft_data.work_buffer = RL_CALLOC(ANALYSIS_WINDOW_SIZE_IN_FRAMES, sizeof(FFTComplex));
    fft_data.prev_spectrum_bin_levels = RL_CALLOC(ANALYSIS_SPECTRUM_BIN_COUNT, sizeof(float));
    fft_data.spectrum_history_levels = RL_CALLOC(ANALYSIS_FFT_HISTORY_FRAME_COUNT, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]));

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
    // Wave wave = LoadWave(RD_DDS_FFM_22K_WAV);
    Wave wave = LoadWave(RD_SHADERTOY_EXPERIMENT_22K_WAV);
    WaveFormat(&wave, SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    AudioStream audio_stream = LoadAudioStream(SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    PlayAudioStream(audio_stream);

    unsigned int wave_cursor = 0;
    int16_t* wave_pcm16 = (int16_t*)wave.data;

    Camera3D camera = {
        .position = (Vector3){1.45625f * 3.0f, 1.345f * 3.0f, -1.36625f * 3.0f},
        .target = (Vector3){0.0f, 0.25f, 0.0f},
        .up = Y_AXIS,
        .fovy = 8.0f,
        .projection = CAMERA_ORTHOGRAPHIC,
    };

    mesh_a = GenMeshPlane(1.0f, 1.0f, LANE_POINT_COUNT - 1, LANE_COUNT - 1);
    // NOTE: FOR USING CUSTOM LANE TOPOLOGY APPROACH WITH "WIRE DRAWS":
    // mesh_a.triangleCount = LINE_SEGMENT_COUNT; // TODO: defeats the purpose of genMesh...
    // mesh_a.indices = RL_CALLOC(LINE_INDEX_COUNT, sizeof(unsigned short)); // TODO: defeats the purpose of genMesh...
    // fill_mesh_indices_lane_topology(mesh_a.indices);

    update_mesh_vertices(mesh_a.vertices, &lane_point_values[0][0]);
    mesh_a.colors = RL_CALLOC(mesh_a.vertexCount, sizeof(Color));

    Texture2D lane_mask_texture = build_lane_mask(mesh_a.texcoords);
    // Texture2D lane_mask_texture = build_lane_mask_glow(mesh_a.texcoords);

    model_a = LoadModelFromMesh(mesh_a);
    unsigned char* saved_colors = model_a.meshes[0].colors;
    model_a.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = lane_mask_texture;
    int saved_texture_id = model_a.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id;
    model_a.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = 0;

    flat_mesh = (Mesh){
        .vertexCount = mesh_a.triangleCount * 3,
        .triangleCount = mesh_a.triangleCount,
        .vertices = RL_CALLOC(mesh_a.triangleCount * 3 * 3, sizeof(float)),
        .normals = RL_CALLOC(mesh_a.triangleCount * 3 * 3, sizeof(float)),
        .colors = RL_CALLOC(mesh_a.triangleCount * 3, sizeof(Color)),
        .indices = NULL, //NOTE: explicit here for didactic
    };
    flat_model = LoadModelFromMesh(flat_mesh);

    fill_mesh_colors(colors);
    expand_mesh_colors_flat(flat_colors, colors, mesh_a.indices);

    update_mesh_vertices(vertices, &lane_point_values[0][0]);
    update_mesh_normals_smooth(normals, vertices);

    update_mesh_vertices_flat(flat_vertices, vertices, mesh_a.indices);
    update_mesh_normals_flat(flat_normals, flat_vertices);

    build_mesh_smooth(&mesh_a, vertices, normals, colors);

    // build_mesh_smooth(&mesh_b, vertices, normals, colors);
    // build_mesh_flat(&flat_mesh, flat_vertices, flat_normals, flat_colors);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        int audio_dirty = 0;
        while (IsAudioStreamProcessed(audio_stream)) {
            for (int i = 0; i < AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES; i++) {
                chunk_samples[i] = wave_pcm16[wave_cursor];
                if (++wave_cursor >= wave.frameCount) {
                    wave_cursor = 0;
                }
            }

            UpdateAudioStream(audio_stream, chunk_samples, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);

            for (int i = 0; i < ANALYSIS_WINDOW_SIZE_IN_FRAMES; i++) {
                analysis_window_samples[i] = (float)chunk_samples[i] / ANALYSIS_PCM16_UPPER_BOUND;
            }

            apply_blackman_window(&fft_data, analysis_window_samples);
            shz_fft((shz_complex_t*)fft_data.work_buffer, (size_t)ANALYSIS_WINDOW_SIZE_IN_FRAMES);
            build_spectrum(&fft_data);

            float* spectrum_bin_levels =
                fft_data.spectrum_history_levels[(fft_data.history_pos - 1 + ANALYSIS_FFT_HISTORY_FRAME_COUNT) % ANALYSIS_FFT_HISTORY_FRAME_COUNT];
            advance_lane_history(&lane_point_values[0][0]);
            for (int i = 0; i < LANE_POINT_COUNT; i++) {
                lane_point_values[0][i] = spectrum_bin_levels[(i * (ANALYSIS_SPECTRUM_BIN_COUNT - 1)) / (LANE_POINT_COUNT - 1)];
            }
            update_onset_interpolation_factor(&fft_data);
            audio_dirty = 1;
        }

        if (audio_dirty) {
            update_mesh_vertices(mesh_a.vertices, &lane_point_values[0][0]);
        }
        update_camera_orbit(&camera, GetFrameTime());
        update_light_constants();

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);
        rlSetLineWidth(LINE_WIDTH_RASTER_PIXELS);
        rlSetPointSize(POINT_SIZE_RASTER_PIXELS);

        // NOTE: FOR USING CUSTOM LANE TOPOLOGY APPROACH WITH "WIRE DRAWS":
        // rlEnableStatePointer(GL_COLOR_ARRAY, mesh_a.colors);
        // rlEnableStatePointer(GL_VERTEX_ARRAY, mesh_a.vertices);
        // glDrawElements(RL_LINES, mesh_a.triangleCount * 2, GL_UNSIGNED_SHORT, (const unsigned short*)mesh_a.indices);
        // rlDisableStatePointer(GL_COLOR_ARRAY);
        // rlDisableStatePointer(GL_VERTEX_ARRAY);

        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (const GLfloat[]){0.2f, 0.2f, 0.2f, 1.0f});
        glLightfv(GL_LIGHT0, GL_AMBIENT, (const GLfloat[]){0.0f, 0.0f, 0.0f, 1.0f});
        // glLightfv(GL_LIGHT0, GL_DIFFUSE, (const GLfloat[]){0.6f, 0.6f, 0.6f, 1.0f});
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
        glLightfv(GL_LIGHT0, GL_POSITION, (const GLfloat[]){1.330f, 1.345f, -1.418f, 1.0f});
        DrawModelEx(model_a, MIDDLE, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);
        glDisable(GL_LIGHTING);

        model_a.meshes[0].colors = NULL;
        // model_a.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = saved_texture_id;
        // rlDisableDepthTest();
        // rlDisableBackfaceCulling();
        // DrawModelEx(model_a, MIDDLE, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);
        // rlEnableBackfaceCulling();
        // rlEnableDepthTest();
        // model_a.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = 0;

        // DrawModelPointsEx(model_a, TOP, Y_AXIS, 0.0f, DEFAULT_SCALE, BLUE);
        // DrawModelWiresEx(model_a, BOTTOM, Y_AXIS, 0.0f, DEFAULT_SCALE, MAGENTA);

        model_a.meshes[0].colors = saved_colors;

        EndMode3D();
        DrawFPS(550, 440);
        EndDrawing();
    }

    UnloadTexture(lane_mask_texture);
    UnloadModel(model_a);
    UnloadAudioStream(audio_stream);
    UnloadWave(wave);
    CloseAudioDevice();
    RL_FREE(fft_data.spectrum_history_levels);
    RL_FREE(fft_data.prev_spectrum_bin_levels);
    RL_FREE(fft_data.work_buffer);
    CloseWindow();
    return 0;
}

static void update_onset_interpolation_factor(FFTData* fft_data) {
    // librosa onset_strength convention:
    // https://librosa.org/doc/main/generated/librosa.onset.onset_strength.html
    // TODO: onset_detect would be a later peak-pick stage is more complicated...
    // https://librosa.org/doc/main/generated/librosa.onset.onset_detect.html
    if (fft_data->frame_index <= ONSET_LAG_FRAMES) {
        return;
    }

    int current_index = (fft_data->history_pos - 1 + ANALYSIS_FFT_HISTORY_FRAME_COUNT) % ANALYSIS_FFT_HISTORY_FRAME_COUNT;
    int lag_index = (fft_data->history_pos - 1 - ONSET_LAG_FRAMES + ANALYSIS_FFT_HISTORY_FRAME_COUNT) % ANALYSIS_FFT_HISTORY_FRAME_COUNT;
    float* spectrum = fft_data->spectrum_history_levels[current_index];
    float* lag_spectrum = fft_data->spectrum_history_levels[lag_index];
    float flux_sum = 0.0f;
    for (int i = 0; i < ANALYSIS_SPECTRUM_BIN_COUNT; i++) {
        float ref = lag_spectrum[i];
        for (int j = -ONSET_ENVELOPE_RADIUS; j <= ONSET_ENVELOPE_RADIUS; j++) {
            int k = i + j;
            if (k < 0 || k >= ANALYSIS_SPECTRUM_BIN_COUNT) {
                continue;
            }
            ref = FMAXF(ref, lag_spectrum[k]);
        }
        float flux = spectrum[i] - ref;
        if (flux > 0.0f) {
            flux_sum += flux;
        }
    }
    float onset_strength = flux_sum / (float)ANALYSIS_SPECTRUM_BIN_COUNT;
    float onset_strength_normalized = CLAMP((onset_strength - ONSET_STRENGTH_MIN) / (ONSET_STRENGTH_MAX - ONSET_STRENGTH_MIN), 0.0f, 1.0f);
    float onset_rate = ONSET_DECAY_RATE;
    if (onset_strength_normalized > onset_interpolation_factor) {
        onset_rate = ONSET_ATTACK_RATE;
    }
    onset_interpolation_factor = LERP(onset_interpolation_factor, onset_strength_normalized, onset_rate);
    onset_interpolation_factor = CLAMP(onset_interpolation_factor, 0.0f, 1.0f);
}

static void update_light_constants(void) {
    float light0_diffuse_strength = LERP(LIGHT0_DIFFUSE_MIN, LIGHT0_DIFFUSE_MAX, onset_interpolation_factor);
    light0_diffuse[0] = light0_diffuse_strength;
    light0_diffuse[1] = light0_diffuse_strength;
    light0_diffuse[2] = light0_diffuse_strength;
    light0_diffuse[3] = 1.0f;
}
