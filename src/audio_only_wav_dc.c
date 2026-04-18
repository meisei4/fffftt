#include "fffftt.h"

static const char* domain = "AUDIO-ONLY-WAV-DC";

int main(void) {
    int16_t chunk_samples[AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES] = {0};

    unsigned int device_period_frames = 0;
    //SetTraceLogLevel(LOG_WARNING); // TODO: note this should be commented out for testing logs on
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    InitAudioDevice();
    device_period_frames = GetAudioDevicePeriodSizeInFrames();
    SetTargetFPS(60);
    SetAudioStreamBufferSizeDefault(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
    Wave wave = LoadWave(RD_COUNTRY_22K_WAV);
    WaveFormat(&wave, SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    AudioStream audio_stream = LoadAudioStream(SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    PlayAudioStream(audio_stream);
    size_t wave_cursor = 0;
    size_t wave_sample_count = (size_t)wave.frameCount * (size_t)AUDIO_DEVICE_CHANNELS;
    int16_t* wave_pcm16 = (int16_t*)wave.data;

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        while (IsAudioStreamProcessed(audio_stream)) {
            for (int i = 0; i < AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES; i++) {
                chunk_samples[i] = wave_pcm16[wave_cursor];
                if (++wave_cursor >= wave_sample_count) {
                    wave_cursor = 0;
                }
            }

            UpdateAudioStream(audio_stream, chunk_samples, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawText(TextFormat("FILE: %.*s", (int)(sizeof(RD_COUNTRY_22K_WAV) - 1), RD_COUNTRY_22K_WAV), 16, 16, 20, WHITE);
        DrawText(TextFormat("PERIOD: %u", device_period_frames), 16, 44, 20, WHITE);
        EndDrawing();
    }

    UnloadAudioStream(audio_stream);
    UnloadWave(wave);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
