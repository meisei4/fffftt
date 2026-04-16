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
        .position = (Vector3){1.45625f * 2.0f, 1.345f * 2.0f, -1.36625f * 2.0f},
        .target = (Vector3){0.0f, 0.25f, 0.0f},
        .up = (Vector3){0.0f, 1.0f, 0.0f},
        .fovy = 5.0f,
        .projection = CAMERA_ORTHOGRAPHIC,
    };

    // mesh = (Mesh){
    //     .vertexCount = MESH_VERTEX_COUNT,
    //     .triangleCount = LINE_SEGMENT_COUNT,
    //     .vertices = RL_CALLOC(MESH_VERTEX_COUNT * 3, sizeof(float)),
    //     .indices = RL_CALLOC(LINE_INDEX_COUNT, sizeof(unsigned short)),
    // };

    mesh = GenMeshPlane(1.0f, 1.0f, LANE_POINT_COUNT - 1, LANE_COUNT - 1);
    mesh.triangleCount = LINE_SEGMENT_COUNT; // TODO: defeats the purpose of genMesh...

    // mesh.indices = RL_CALLOC(LINE_INDEX_COUNT, sizeof(unsigned short)); // TODO: defeats the purpose of genMesh...
    fill_mesh_indices_lane_topology(mesh.indices);

    update_terrain_mesh_vertices(mesh.vertices, &lane_point_samples[0][0]);
    mesh.colors = RL_CALLOC(mesh.vertexCount, sizeof(Color));
    fill_mesh_colors((Color*)mesh.colors);

    Model model = LoadModelFromMesh(mesh);
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
        rlSetLineWidth(LINE_WIDTH_RASTER_PIXELS);
        rlSetPointSize(POINT_SIZE_RASTER_PIXELS);

        rlEnableStatePointer(GL_COLOR_ARRAY, mesh.colors);
        rlEnableStatePointer(GL_VERTEX_ARRAY, mesh.vertices);
        glDrawElements(RL_LINES, mesh.triangleCount * 2, GL_UNSIGNED_SHORT, (const unsigned short*)mesh.indices);
        rlDisableStatePointer(GL_COLOR_ARRAY);
        rlDisableStatePointer(GL_VERTEX_ARRAY);

        // DrawModelEx(model, (Vector3){0.0f}, (Vector3){0.0f, 1.0f, 0.0f}, 0.0f, (Vector3){1.0f, 1.0f, 1.0f}, WHITE);
        // DrawModelWiresEx(model, (Vector3){0.0f}, (Vector3){0.0f, 1.0f, 0.0f}, 0.0f, (Vector3){1.0f, 1.0f, 1.0f}, BLUE);
        // DrawModelPointsEx(model, (Vector3){0.0f}, (Vector3){0.0f, 1.0f, 0.0f}, 0.0f, (Vector3){1.0f, 1.0f, 1.0f}, MAGENTA);

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
