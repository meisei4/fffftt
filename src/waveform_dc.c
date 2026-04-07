#include "audio_spectrum_analyzer.h"
#include "raylib.h"
#include <stdint.h>
#include <stdlib.h>

static const char* domain = "WAVEFORM-DC";

static Wave wav = {0};
static AudioStream audio_stream = {0};
static size_t wav_cursor = 0;
static int16_t* wav_pcm16 = NULL;
static int16_t chunk_samples[WAVEFORM_AUDIO_STREAM_RING_BUFFER_SIZE] = {0};

int main(void) {
    float audio_samples[WAVEFORM_WINDOW_SIZE] = {0};
    float waveform_data[WAVEFORM_BUFFER_SIZE] = {0};

    SetTraceLogLevel(LOG_WARNING); // TODO: note this should be commented out for testing logs on
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(WAVEFORM_AUDIO_STREAM_RING_BUFFER_SIZE);
    wav = LoadWave("/rd/shadertoy_experiment_22050hz_pcm16_mono.wav");
    WaveFormat(&wav, SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    audio_stream = LoadAudioStream(SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    PlayAudioStream(audio_stream);
    wav_pcm16 = (int16_t*)wav.data;
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        while (IsAudioStreamProcessed(audio_stream)) {
            for (int i = 0; i < WAVEFORM_AUDIO_STREAM_RING_BUFFER_SIZE; i++) {
                chunk_samples[i] = wav_pcm16[wav_cursor];
                if (++wav_cursor >= wav.frameCount) {
                    wav_cursor = 0;
                }
            }

            UpdateAudioStream(audio_stream, chunk_samples, WAVEFORM_AUDIO_STREAM_RING_BUFFER_SIZE);

            for (int i = 0; i < WAVEFORM_WINDOW_SIZE; i++) {
                audio_samples[i] = (float)chunk_samples[WAVEFORM_WINDOW_SIZE + i] / PCM_SAMPLE_MAX_F;
            }

            update_waveform_data(waveform_data, audio_samples);
        }

        BeginDrawing();
        ClearBackground(BLACK);
        render_waveform_frame(waveform_data);
        DrawFPS(520, 400);
        EndDrawing();
    }

    UnloadAudioStream(audio_stream);
    UnloadWave(wav);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
