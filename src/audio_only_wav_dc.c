#include "fffftt.h"

static const char* domain = "AUDIO-ONLY-WAV-DC";

int main(void) {
    // SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    InitAudioDevice();
    SetTargetFPS(60);
    SetAudioStreamBufferSizeDefault(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
    wave = LoadWave(RD_COUNTRY_22K_WAV);
    WaveFormat(&wave, SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    audio_stream = LoadAudioStream(SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    PlayAudioStream(audio_stream);

    wave_pcm16 = (int16_t*)wave.data;

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
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawText(TextFormat("FILE: %.*s", (int)(sizeof(RD_COUNTRY_22K_WAV) - 1), RD_COUNTRY_22K_WAV), 16, 16, 20, WHITE);
        DrawText(TextFormat("PERIOD: %u", AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES), 16, 44, 20, WHITE);
        EndDrawing();
    }

    UnloadAudioStream(audio_stream);
    UnloadWave(wave);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
