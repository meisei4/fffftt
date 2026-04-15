#define FFFFTT_PROFILE_SOUND_ENVELOPE_ISO
#include "fffftt.h"
#include "rlgl.h"
#include <GL/gl.h>

#define LINE_WIDTH_RASTER_PIXELS 2.0f

static const char* domain = "SOUND-ENVELOPE-DC";

static Vector3 envelope_mesh_vertices[LANE_COUNT][LANE_POINT_COUNT] = {0};
static float lane_point_samples[LANE_COUNT][LANE_POINT_COUNT] = {0};
static float waveform_window_samples[WINDOW_SIZE] = {0};

int main(void) {
    int16_t chunk_samples[AUDIO_STREAM_RING_BUFFER_SIZE] = {0};

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_STREAM_RING_BUFFER_SIZE);
    Wave wave = LoadWave(RD_SHADERTOY_EXPERIMENT_22K_WAV);
    WaveFormat(&wave, SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    AudioStream stream = LoadAudioStream(SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    PlayAudioStream(stream);

    size_t wave_cursor = 0;
    int16_t* pcm_data = (int16_t*)wave.data;

    SetTargetFPS(15);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        while (IsAudioStreamProcessed(stream)) {
            for (int i = 0; i < AUDIO_STREAM_RING_BUFFER_SIZE; i++) {
                chunk_samples[i] = pcm_data[wave_cursor];
                if (++wave_cursor >= wave.frameCount) {
                    wave_cursor = 0;
                }
            }

            UpdateAudioStream(stream, chunk_samples, AUDIO_STREAM_RING_BUFFER_SIZE);

            for (int i = 0; i < WINDOW_SIZE; i++) {
                waveform_window_samples[i] = (float)chunk_samples[WINDOW_SIZE + i] / PCM_SAMPLE_MAX_F;
            }
        }

        advance_lane_history(&lane_point_samples[0][0]);
        smooth_front_lane(&lane_point_samples[0][0], waveform_window_samples);
        update_envelope_mesh_vertices_isometric(&envelope_mesh_vertices[0][0], &lane_point_samples[0][0]);

        BeginDrawing();
        ClearBackground(BLACK);

        rlSetLineWidth(LINE_WIDTH_RASTER_PIXELS); //TODO: this is on custom branch...? i think raylib_dc needs support investigation
        for (int i = 0; i < LANE_COUNT; i++) {
            rlEnableStatePointer(GL_VERTEX_ARRAY, envelope_mesh_vertices[i]);
            rlDrawVertexArrayCustom(0, LANE_POINT_COUNT, GL_LINE_STRIP);
        }

        EndDrawing();
    }

    UnloadAudioStream(stream);
    UnloadWave(wave);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
