#define FFFFTT_PROFILE_SOUND_ENVELOPE_ISO
#include "fffftt.h"
#include <GL/gl.h>

#define LINE_WIDTH_RASTER_PIXELS 2.0f

static const char* domain = "SOUND-ENVELOPE-DC";

static Vector3 envelope_mesh_vertices[LANE_COUNT][LANE_POINT_COUNT] = {0};

int main(void) {
    // SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    font = LoadFont(RD_FONT);

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
    load_audio_tracks();
    set_audio_track(DEFAULT_AUDIO_TRACK_SHADERTOY_EXPERIMENT);
    audio_stream = LoadAudioStream(SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    PlayAudioStream(audio_stream);

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
        }

        update_envelope_mesh_vertices_isometric(&envelope_mesh_vertices[0][0]);
        update_audio_track_cycle();

        BeginDrawing();
        ClearBackground(BLACK);

        rlSetLineWidth(LINE_WIDTH_RASTER_PIXELS);
        for (int i = 0; i < LANE_COUNT; i++) {
            rlEnableStatePointer(GL_VERTEX_ARRAY, envelope_mesh_vertices[i]);
            glDrawArrays(GL_LINE_STRIP, 0, LANE_POINT_COUNT);
        }
        if (is_paused) {
            float blink = SINF((float)GetTime() * WAVE_CURSOR_BLINK_RATE);
            Color marker_color = CLITERAL(Color){
                (unsigned char)((float)NEON_CARROT.r * (0.725f + 0.275f * blink)),
                (unsigned char)((float)NEON_CARROT.g * (0.725f + 0.275f * blink)),
                (unsigned char)((float)NEON_CARROT.b * (0.725f + 0.275f * blink)),
                (unsigned char)(175.5f + 79.5f * blink),
            };
            Vector2 marker_vertices[LANE_POINT_COUNT] = {0};
            int marker_lane_index = seek_delta_chunks;

            if (marker_lane_index < 0 || marker_lane_index >= LANE_COUNT) {
                float paused_lane_point_values[LANE_POINT_COUNT] = {0};
                int geometry_lane_index = (marker_lane_index < 0) ? 0 : (LANE_COUNT - 1);
                float lane_offset = (float)geometry_lane_index * ISOMETRIC_LANE_SPACING;
                float outside_shift_x = (marker_lane_index < 0) ? (ISOMETRIC_LANE_SPACING * ISOMETRIC_ZOOM) : (-ISOMETRIC_LANE_SPACING * ISOMETRIC_ZOOM);
                float outside_shift_y =
                    (marker_lane_index < 0) ? (0.5f * ISOMETRIC_LANE_SPACING * ISOMETRIC_ZOOM) : (-0.5f * ISOMETRIC_LANE_SPACING * ISOMETRIC_ZOOM);

                for (int i = 0; i < LANE_POINT_COUNT; i++) {
                    float sample_sum = 0.0f;
                    int sample_index = i * WAVEFORM_SAMPLES_PER_LANE_POINT;
                    for (int j = 0; j < WAVEFORM_SAMPLES_PER_LANE_POINT; j++) {
                        int src = (paused_wave_cursor + sample_index + j) % wave.frameCount;
                        sample_sum += FABSF((float)wave_pcm16[src] / ANALYSIS_PCM16_UPPER_BOUND);
                    }
                    paused_lane_point_values[i] = (sample_sum / (float)WAVEFORM_SAMPLES_PER_LANE_POINT) * FRONT_LANE_SMOOTHING;
                }

                for (int i = 0; i < LANE_POINT_COUNT; i++) {
                    float grid_x = (float)i - lane_offset;
                    float grid_y = 0.5f * ((float)i + lane_offset) - paused_lane_point_values[i] * AMPLITUDE_Y_SCALE;
                    marker_vertices[i].x = 0.5f * (float)SCREEN_WIDTH + (grid_x - ISOMETRIC_GRID_CENTER_X) * ISOMETRIC_ZOOM + outside_shift_x;
                    marker_vertices[i].y = 0.5f * (float)SCREEN_HEIGHT - (grid_y - ISOMETRIC_GRID_CENTER_Y) * ISOMETRIC_ZOOM + outside_shift_y;
                }
            } else {
                for (int i = 0; i < LANE_POINT_COUNT; i++) {
                    marker_vertices[i].x = envelope_mesh_vertices[marker_lane_index][i].x;
                    marker_vertices[i].y = envelope_mesh_vertices[marker_lane_index][i].y;
                }
            }

            for (int i = 0; i < LANE_POINT_COUNT - 1; i++) {
                DrawLineEx(marker_vertices[i], marker_vertices[i + 1], LINE_WIDTH_RASTER_PIXELS + 1.0f, marker_color);
            }
        }

        draw_playback_inspection_hud();
        DrawTextEx(font, TextFormat("%2i FPS", GetFPS()), (Vector2){50.0f, 440.0f}, FONT_SIZE, 0.0f, WHITE);
        EndDrawing();
    }

    UnloadAudioStream(audio_stream);
    unload_audio_tracks();
    CloseAudioDevice();
    UnloadFont(font);
    CloseWindow();
    return 0;
}
