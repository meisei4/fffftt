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
    set_audio_track(SHADERTOY_EXPERIMENT);
    audio_stream = LoadAudioStream(SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    PlayAudioStream(audio_stream);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        update_audio_track_cycle();
        update_playback_controls_sound_envelope();
        while (fffftt_audio_process(chunk_samples)) {
            advance_lane_history(&lane_point_values[0][0], LANE_POINT_COUNT);
            smooth_lane(0);
        }

        update_envelope_mesh_vertices_isometric(&envelope_mesh_vertices[0][0]);

        BeginDrawing();
        ClearBackground(BLACK);

        rlSetLineWidth(LINE_WIDTH_RASTER_PIXELS);
        for (int i = 0; i < LANE_COUNT; i++) {
            rlEnableStatePointer(GL_VERTEX_ARRAY, envelope_mesh_vertices[i]);
            glDrawArrays(GL_LINE_STRIP, 0, LANE_POINT_COUNT);
        }
        if (is_paused) {
            Color marker_color = fffftt_wave_cursor_blink_color();
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
                    int sample_index = i * WAVEFORM_SAMPLES_PER_LANE_POINT;
                    int sample_cursor = WRAP_PLUS(wave_cursor, sample_index, wave.frameCount);
                    float sample_sum = fffftt_accumulate_sound_envelope(sample_cursor, WAVEFORM_SAMPLES_PER_LANE_POINT);
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
        EndDrawing();
    }

    UnloadAudioStream(audio_stream);
    unload_audio_track();
    CloseAudioDevice();
    UnloadFont(font);
    CloseWindow();
    return 0;
}
