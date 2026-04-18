#include "fffftt.h"

static const char* domain = "AUDIO-ONLY-MP3-DC";

int main(void) {
    // SetTraceLogLevel(LOG_WARNING); // TODO: note this should be commented out for testing logs on
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    InitAudioDevice();
    unsigned int device_period_frames = GetAudioDevicePeriodSizeInFrames();
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
        DrawText(TextFormat("PERIOD: %u", device_period_frames), 16, 44, 20, WHITE);
        EndDrawing();
    }

    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
