#define FFFFTT_PROFILE_SOUND_ENVELOPE_3D
#include "fffftt.h"
#include "rlgl.h"
#include <GL/gl.h>

#define LINE_WIDTH_RASTER_PIXELS 2.0f

static const char* domain = "SOUND-ENVELOPE-3D-AUDIO-CADENCE-DC";

static Vector3 envelope_mesh_vertices[LANE_COUNT][LANE_POINT_COUNT] = {0};
static float lane_point_samples[LANE_COUNT][LANE_POINT_COUNT] = {0};
static float waveform_window_samples[WINDOW_SIZE] = {0};

int main(void) {
    int16_t chunk_samples[AUDIO_STREAM_RING_BUFFER_SIZE] = {0};

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_STREAM_RING_BUFFER_SIZE);
    Wave wav = LoadWave(RD_SHADERTOY_EXPERIMENT_22K_WAV);
    WaveFormat(&wav, SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    AudioStream audio_stream = LoadAudioStream(SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    PlayAudioStream(audio_stream);

    size_t wav_cursor = 0;
    int16_t* wav_pcm16 = (int16_t*)wav.data;

    Camera3D camera = {
        .position = (Vector3){-1.093f, 1.126f, 1.165f}, //TODO: manually tuned alignment...
        .target = (Vector3){0.0f, 0.25f, 0.0f},
        .up = (Vector3){0.0f, 1.0f, 0.0f},
        .fovy = 3.111f, //TODO: manually tuned alignment... 2D software isometric projection -> true 3D orthographic projection is tough
        .projection = CAMERA_ORTHOGRAPHIC,
    };

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
                waveform_window_samples[i] = (float)chunk_samples[WINDOW_SIZE + i] / PCM_SAMPLE_MAX_F;
            }

            advance_lane_history(&lane_point_samples[0][0]);
            smooth_front_lane(&lane_point_samples[0][0], waveform_window_samples);
        }

        update_envelope_mesh_vertices(&envelope_mesh_vertices[0][0], &lane_point_samples[0][0]);
        update_camera_orbit(&camera, (float)GetFrameTime());

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);

        rlSetLineWidth(LINE_WIDTH_RASTER_PIXELS);
        for (int i = 0; i < LANE_COUNT; i++) {
            rlEnableStatePointer(GL_VERTEX_ARRAY, envelope_mesh_vertices[i]);
            glDrawArrays(GL_LINE_STRIP, 0, LANE_POINT_COUNT);
        }

        EndMode3D();
        EndDrawing();
    }

    UnloadAudioStream(audio_stream);
    UnloadWave(wav);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
