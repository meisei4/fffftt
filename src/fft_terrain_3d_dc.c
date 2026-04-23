#include "fffftt.h"
#include "../sh4zam/include/sh4zam/shz_sh4zam.h"
#include "raylib.h"
#include "rlgl.h"
#include <GL/gl.h>

#define LINE_WIDTH_RASTER_PIXELS 2.0f
#define POINT_SIZE_RASTER_PIXELS 3.0f

static const char* domain = "FFT-TERRAIN-3D-DC";

static Mesh mesh_a = {0};

static Model model_a = {0};

static float lane_point_values[LANE_COUNT][LANE_POINT_COUNT] = {0};

int main(void) {
    FFTData fft_data = {0};
    float analysis_window_samples[ANALYSIS_WINDOW_SIZE_IN_FRAMES] = {0};
    int16_t chunk_samples[AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES] = {0};

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    fft_data.tapback_pos = ANALYSIS_TAPBACK_POS_DEFAULT;
    fft_data.work_buffer = RL_CALLOC(ANALYSIS_WINDOW_SIZE_IN_FRAMES, sizeof(FFTComplex));
    fft_data.prev_spectrum_bin_levels = RL_CALLOC(ANALYSIS_SPECTRUM_BIN_COUNT, sizeof(float));
    fft_data.spectrum_history_levels = RL_CALLOC(ANALYSIS_FFT_HISTORY_FRAME_COUNT, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]));

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
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

    update_terrain_mesh_vertices(mesh_a.vertices, &lane_point_values[0][0]);
    mesh_a.colors = RL_CALLOC(mesh_a.vertexCount, sizeof(Color));
    fill_mesh_colors((Color*)mesh_a.colors);

    Texture2D lane_mask_texture = build_lane_mask(mesh_a.texcoords);
    // Texture2D lane_mask_texture = build_lane_mask_glow(mesh_a.texcoords);

    model_a = LoadModelFromMesh(mesh_a);
    model_a.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = lane_mask_texture;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

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
        }

        update_terrain_mesh_vertices(mesh_a.vertices, &lane_point_values[0][0]);
        update_camera_orbit(&camera, GetFrameTime());

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

        rlDisableDepthMask();
        rlDisableBackfaceCulling();
        DrawModelEx(model_a, (Vector3){0.0f}, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);
        rlEnableBackfaceCulling();
        rlEnableDepthMask();

        // DrawModelEx(model_a, (Vector3){0.0f}, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);
        // DrawModelWiresEx(model_a, (Vector3){0.0f}, Y_AXIS, 0.0f, DEFAULT_SCALE, BLUE);
        // DrawModelPointsEx(model_a, (Vector3){0.0f}, Y_AXIS, 0.0f, DEFAULT_SCALE, MAGENTA);

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
