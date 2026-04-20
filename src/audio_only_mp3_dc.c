#include "fffftt.h"

static const char* domain = "AUDIO-ONLY-MP3-DC";

// TODO: raudio.c, this isnt helpful at all, remove the whole device controls completely from the build, its literally a single line in config.h, just update it...
// unsigned int GetAudioDevicePeriodSizeInFrames(void) {
//     if (!AUDIO.System.isReady) return 0;
//     return AUDIO.System.device.playback.internalPeriodSizeInFrames*AUDIO.System.device.playback.internalPeriods;
// }

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    InitAudioDevice();
    SetTargetFPS(60);
    Music music = LoadMusicStream(RD_COUNTRY_STEREO_MP3);
    PlayMusicStream(music);
    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        UpdateMusicStream(music);
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText(TextFormat("FILE: %.*s", (int)(sizeof(RD_COUNTRY_STEREO_MP3) - 1), RD_COUNTRY_STEREO_MP3), 16, 16, 20, WHITE);
        DrawText(TextFormat("PERIOD: %u", AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES), 16, 44, 20, WHITE);
        EndDrawing();
    }

    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
