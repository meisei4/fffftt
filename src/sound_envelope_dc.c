#include "audio_spectrum_analyzer.h"
#include "raylib.h"
#include "rlgl.h"
#include <GL/gl.h>
#include <math.h>
#include <stdint.h>

static const char* domain = "SOUND-ENVELOPE-DC";

#define LANE_COUNT 5
#define LANE_POINT_COUNT 64
#define WAVEFORM_SAMPLES_PER_LANE_POINT (WAVEFORM_BUFFER_SIZE / LANE_POINT_COUNT)

#define AMPLITUDE_Y_SCALE 120.0f
#define LANE_SPACING 9.0f
#define ISOMETRIC_ZOOM 3.0f
#define ISOMETRIC_GRID_CENTER_X (0.5f * (-(float)(LANE_COUNT - 1) * LANE_SPACING + (float)(LANE_POINT_COUNT - 1)))
#define ISOMETRIC_GRID_CENTER_Y (0.25f * ((float)(LANE_POINT_COUNT - 1) + (float)(LANE_COUNT - 1) * LANE_SPACING))
#define ENVELOPE_LINE_WIDTH 2.0f
#define FRONT_LANE_SMOOTHING 0.4f

static Vector3 envelope_mesh_vertices[LANE_COUNT][LANE_POINT_COUNT] = {0};
static float lane_point_samples[LANE_COUNT][LANE_POINT_COUNT] = {0};
static float next_lane_point_samples[LANE_COUNT][LANE_POINT_COUNT] = {0};
static float waveform_window_samples[WAVEFORM_WINDOW_SIZE] = {0};

static void update_envelope_mesh_vertices(void);

int main(void) {
    int16_t chunk_samples[WAVEFORM_AUDIO_STREAM_RING_BUFFER_SIZE] = {0};

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(WAVEFORM_AUDIO_STREAM_RING_BUFFER_SIZE);
    Wave wave = LoadWave("/rd/shadertoy_experiment_22050hz_pcm16_mono.wav");
    WaveFormat(&wave, SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    AudioStream stream = LoadAudioStream(SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    PlayAudioStream(stream);

    size_t wave_cursor = 0;
    int16_t* pcm_data = (int16_t*)wave.data;

    SetTargetFPS(15);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        while (IsAudioStreamProcessed(stream)) {
            for (int i = 0; i < WAVEFORM_AUDIO_STREAM_RING_BUFFER_SIZE; i++) {
                chunk_samples[i] = pcm_data[wave_cursor];
                if (++wave_cursor >= wave.frameCount) {
                    wave_cursor = 0;
                }
            }

            UpdateAudioStream(stream, chunk_samples, WAVEFORM_AUDIO_STREAM_RING_BUFFER_SIZE);

            for (int i = 0; i < WAVEFORM_WINDOW_SIZE; i++) {
                waveform_window_samples[i] = (float)chunk_samples[WAVEFORM_WINDOW_SIZE + i] / PCM_SAMPLE_MAX_F;
            }
        }

        update_envelope_mesh_vertices();

        BeginDrawing();
        ClearBackground(BLACK);

        rlSetLineWidth(ENVELOPE_LINE_WIDTH);
        for (int lane_index = 0; lane_index < LANE_COUNT; lane_index++) {
            rlEnableStatePointer(GL_VERTEX_ARRAY, envelope_mesh_vertices[lane_index]);
            rlDrawVertexArrayCustom(0, LANE_POINT_COUNT, GL_LINE_STRIP);
        }

        EndDrawing();
    }

    UnloadAudioStream(stream);
    UnloadWave(wave);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

static void update_envelope_mesh_vertices(void) {
    for (int i = LANE_COUNT - 2; i >= 0; i--) {
        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            next_lane_point_samples[i + 1][j] = lane_point_samples[i][j];
        }
    }

    for (int i = 0; i < LANE_POINT_COUNT; i++) {
        float amplitude_sum = 0.0f;
        for (int j = 0; j < WAVEFORM_SAMPLES_PER_LANE_POINT; j++) {
            amplitude_sum += fabsf(waveform_window_samples[i * WAVEFORM_SAMPLES_PER_LANE_POINT + j]);
        }

        float cur_lane_point_sample = lane_point_samples[0][i];
        next_lane_point_samples[0][i] = cur_lane_point_sample + (amplitude_sum / (float)WAVEFORM_SAMPLES_PER_LANE_POINT - cur_lane_point_sample);
        next_lane_point_samples[0][i] *= FRONT_LANE_SMOOTHING;
    }

    for (int i = 0; i < LANE_COUNT; i++) {
        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            lane_point_samples[i][j] = next_lane_point_samples[i][j];
        }
    }

    for (int i = 0; i < LANE_COUNT; i++) {
        float lane_offset = (float)i * LANE_SPACING;

        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            float lane_point_sample = lane_point_samples[i][j];
            float grid_x = (float)j - lane_offset;
            float grid_y = 0.5f * ((float)j + lane_offset) - lane_point_sample * AMPLITUDE_Y_SCALE;
            float x = 0.5f * (float)SCREEN_WIDTH + (grid_x - ISOMETRIC_GRID_CENTER_X) * ISOMETRIC_ZOOM;
            float y = 0.5f * (float)SCREEN_HEIGHT - (grid_y - ISOMETRIC_GRID_CENTER_Y) * ISOMETRIC_ZOOM;
            envelope_mesh_vertices[i][j].x = x;
            envelope_mesh_vertices[i][j].y = y;
            envelope_mesh_vertices[i][j].z = 0.0f;
        }
    }
}
