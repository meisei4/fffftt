#define FFFFTT_PROFILE_SOUND_ENVELOPE_3D
#include "fffftt.h"
#include <GL/gl.h>

#define LINE_WIDTH_RASTER_PIXELS 2.0f

static const char* domain = "SOUND-ENVELOPE-3D-DC";

static Vector3 envelope_mesh_vertices[LANE_COUNT][LANE_POINT_COUNT] = {0};

int main(void) {
    // SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    font = LoadFont(RD_FONT);

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
    LOAD_AUDIO_TRACK(DEFAULT_AUDIO_TRACK_SHADERTOY_EXPERIMENT);
    WaveFormat(&wave, SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    audio_stream = LoadAudioStream(SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    PlayAudioStream(audio_stream);
    wave_pcm16 = (int16_t*)wave.data;

    Camera3D camera = {
        .position = (Vector3){-1.093f, 1.126f, 1.165f}, //TODO: manually tuned alignment...
        .target = (Vector3){0.0f, 0.25f, 0.0f},
        .up = Y_AXIS,
        .fovy = 3.111f, //TODO: manually tuned alignment... 2D software isometric projection -> true 3D orthographic projection is tough
        .projection = CAMERA_ORTHOGRAPHIC,
    };

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        update_playback_controls_sound_envelope();
        while (!is_paused && IsAudioStreamProcessed(audio_stream)) {
            for (int i = 0; i < AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES; i++) {
                chunk_samples[i] = wave_pcm16[wave_cursor];
                if (++wave_cursor >= wave.frameCount) {
                    wave_cursor = 0;
                }
            }

            UpdateAudioStream(audio_stream, chunk_samples, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);

            for (int i = 0; i < ANALYSIS_WINDOW_SIZE_IN_FRAMES; i++) {
                analysis_window_samples[i] = (float)chunk_samples[i] / ANALYSIS_PCM16_UPPER_BOUND;
            }
            advance_lane_history(&lane_point_values[0][0], LANE_POINT_COUNT);
            smooth_lane(0);
            //TODO: to avoid smoothing comment out the above and uncomment the below
            // for (int i = 0; i < LANE_POINT_COUNT; i++) {
            //     lane_point_values[0][i] = analysis_window_samples[(i * (ANALYSIS_WINDOW_SIZE_IN_FRAMES - 1)) / (LANE_POINT_COUNT - 1)];
            // }
        }

        update_envelope_mesh_vertices(&envelope_mesh_vertices[0][0]);
        update_audio_track_cycle();
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
        draw_playback_inspection_hud();
        DrawTextEx(font, TextFormat("%2i FPS", GetFPS()), (Vector2){50.0f, 440.0f}, FONT_SIZE, 0.0f, WHITE);
        EndDrawing();
    }

    UnloadAudioStream(audio_stream);
    UnloadWave(wave);
    CloseAudioDevice();
    UnloadFont(font);
    CloseWindow();
    return 0;
}
