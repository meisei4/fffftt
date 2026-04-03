#include "audio_spectrum_analyzer.h"
#include <stdint.h>

#define WAV_TARGET "/rd/country_22050hz_pcm16_mono.wav"
#define FILENAME_CHAR_COUNT (sizeof(WAV_TARGET) - 1)

static const char* domain = "AUDIO-ONLY-DC";
static AudioStream audio_stream = {0};
static Wave wav = {0};
static size_t wav_cursor = 0;
static size_t wav_sample_count = 0;
static int16_t* wav_pcm16 = NULL;
static int16_t chunk_samples[AUDIO_STREAM_RING_BUFFER_SIZE] = {0};

int main(void) {
    unsigned int device_period_frames = 0;
    //SetTraceLogLevel(LOG_WARNING); // TODO: note this should be commented out for testing logs on
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    InitAudioDevice();
    device_period_frames = GetAudioDevicePeriodSizeInFrames();
    SetTargetFPS(60);
    SetAudioStreamBufferSizeDefault(AUDIO_STREAM_RING_BUFFER_SIZE);
    wav = LoadWave(WAV_TARGET);
    WaveFormat(&wav, SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    audio_stream = LoadAudioStream(SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    PlayAudioStream(audio_stream);
    wav_pcm16 = (int16_t*)wav.data;
    wav_sample_count = (size_t)wav.frameCount * (size_t)MONO;

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        while (IsAudioStreamProcessed(audio_stream)) {
            for (int i = 0; i < AUDIO_STREAM_RING_BUFFER_SIZE; i++) {
                chunk_samples[i] = wav_pcm16[wav_cursor];
                if (++wav_cursor >= wav_sample_count) {
                    wav_cursor = 0;
                }
            }

            UpdateAudioStream(audio_stream, chunk_samples, AUDIO_STREAM_RING_BUFFER_SIZE);
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawText(TextFormat("FILE: %.*s", FILENAME_CHAR_COUNT, WAV_TARGET), 16, 16, 20, WHITE);
        DrawText(TextFormat("PERIOD: %u", device_period_frames), 16, 44, 20, WHITE);
        EndDrawing();
    }

    UnloadAudioStream(audio_stream);
    UnloadWave(wav);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
