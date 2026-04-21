#define FFFFTT_PROFILE_SOUND_ENVELOPE_3D
#include "fffftt.h"
#include "rlgl.h"
#include <GL/gl.h>

#define LINE_WIDTH_RASTER_PIXELS 2.0f

// TODO: current stupid trick for draining my understanding of the worst case scenario: mid-stream pause/resume of raylib/miniaudio stream + AICA buffer
#define MAX_DRAIN_CHUNK_COUNT 3
static int16_t drain_chunk_samples[AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES] = {0};

static const char* domain = "SOUND-ENVELOPE-3D-AUDIO-CADENCE-DC";

static Vector3 envelope_mesh_vertices[LANE_COUNT][LANE_POINT_COUNT] = {0};
static float lane_point_values[LANE_COUNT][LANE_POINT_COUNT] = {0};
static float analysis_window_samples[ANALYSIS_WINDOW_SIZE_IN_FRAMES] = {0};
static bool is_paused = false;
static void rebuild_envelope_history_from_wave(unsigned int wave_cursor, const int16_t* wave_pcm16, unsigned int frame_count);
static void update_playback_controls(AudioStream audio_stream, unsigned int* wave_cursor, const int16_t* wave_pcm16, unsigned int frame_count);

int main(void) {
    int16_t chunk_samples[AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES] = {0};

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
    // Wave wave = LoadWave(RD_DDS_FFM_22K_WAV);
    Wave wave = LoadWave(RD_SHADERTOY_EXPERIMENT_22K_WAV);
    WaveFormat(&wave, SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    AudioStream audio_stream = LoadAudioStream(SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    PlayAudioStream(audio_stream);

    unsigned int wave_cursor = 0;
    int16_t* wave_pcm16 = (int16_t*)wave.data;

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

        update_playback_controls(audio_stream, &wave_cursor, wave_pcm16, wave.frameCount);
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

            advance_lane_history(&lane_point_values[0][0]);
            // smooth_front_lane(&lane_point_values[0][0], analysis_window_samples);
            for (int i = 0; i < LANE_POINT_COUNT; i++) {
                lane_point_values[0][i] = analysis_window_samples[(i * (ANALYSIS_WINDOW_SIZE_IN_FRAMES - 1)) / (LANE_POINT_COUNT - 1)];
            }
        }

        update_envelope_mesh_vertices(&envelope_mesh_vertices[0][0], &lane_point_values[0][0]);
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
        EndDrawing();
    }

    UnloadAudioStream(audio_stream);
    UnloadWave(wave);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

static void rebuild_envelope_history_from_wave(unsigned int wave_cursor, const int16_t* wave_pcm16, unsigned int frame_count) {
    for (int i = 0; i < ANALYSIS_WINDOW_SIZE_IN_FRAMES; i++) {
        unsigned int src = (wave_cursor + i) % frame_count;
        analysis_window_samples[i] = (float)wave_pcm16[src] / ANALYSIS_PCM16_UPPER_BOUND;
    }

    for (int i = 0; i < LANE_COUNT; i++) {
        unsigned int start_frame = (wave_cursor + frame_count - (((unsigned int)i * AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES) % frame_count)) % frame_count;

        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            unsigned int window_sample = (j * (ANALYSIS_WINDOW_SIZE_IN_FRAMES - 1)) / (LANE_POINT_COUNT - 1);
            unsigned int src = (start_frame + window_sample) % frame_count;
            lane_point_values[i][j] = (float)wave_pcm16[src] / ANALYSIS_PCM16_UPPER_BOUND;
        }
    }
}

static void update_playback_controls(AudioStream audio_stream, unsigned int* wave_cursor, const int16_t* wave_pcm16, unsigned int frame_count) {
    int16_t resume_chunk_samples[AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES] = {0};

    if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
        if (!is_paused) {
            is_paused = true;
            // PauseAudioStream(audio_stream);
            // *wave_cursor = (*wave_cursor + frame_count - AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES) % frame_count;
            // rebuild_envelope_history_from_wave(*wave_cursor, wave_pcm16, frame_count);
            *wave_cursor = (*wave_cursor + frame_count - AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES) % frame_count;
            for (int i = 0; i < MAX_DRAIN_CHUNK_COUNT; i++) {
                while (!IsAudioStreamProcessed(audio_stream)) {
                    /* KEEP DRAINING! */
                }
                UpdateAudioStream(audio_stream, drain_chunk_samples, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
            }
            PauseAudioStream(audio_stream);
            rebuild_envelope_history_from_wave(*wave_cursor, wave_pcm16, frame_count);
        } else {
            is_paused = false;
            // ResumeAudioStream(audio_stream);
            PlayAudioStream(audio_stream);
            while (IsAudioStreamProcessed(audio_stream)) {
                for (int i = 0; i < AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES; i++) {
                    resume_chunk_samples[i] = wave_pcm16[*wave_cursor];
                    if (++(*wave_cursor) >= frame_count) {
                        *wave_cursor = 0;
                    }
                }
                UpdateAudioStream(audio_stream, resume_chunk_samples, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
            }
        }
    }
    if (is_paused && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
        *wave_cursor = (*wave_cursor + frame_count - AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES) % frame_count;
        rebuild_envelope_history_from_wave(*wave_cursor, wave_pcm16, frame_count);
    } else if (is_paused && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
        *wave_cursor = (*wave_cursor + AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES) % frame_count;
        rebuild_envelope_history_from_wave(*wave_cursor, wave_pcm16, frame_count);
    }
}
