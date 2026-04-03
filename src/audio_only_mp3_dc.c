#include "audio_spectrum_analyzer.h"

#define MP3_TARGET "/rd/country_44100hz_128kbps_stereo.mp3"
#define FILENAME_CHAR_COUNT (sizeof(MP3_TARGET) - 1)

static const char* domain = "AUDIO-ONLY-DC";
static Music music = {0};

int main(void) {
    unsigned int device_period_frames = 0;
    // SetTraceLogLevel(LOG_WARNING); // TODO: note this should be commented out for testing logs on
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    InitAudioDevice();
    device_period_frames = GetAudioDevicePeriodSizeInFrames();
    SetTargetFPS(60);
    music = LoadMusicStream(MP3_TARGET);
    PlayMusicStream(music);
    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        UpdateMusicStream(music);
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText(TextFormat("FILE: %.*s", FILENAME_CHAR_COUNT, MP3_TARGET), 16, 16, 20, WHITE);
        DrawText(TextFormat("PERIOD: %u", device_period_frames), 16, 44, 20, WHITE);
        EndDrawing();
    }

    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
