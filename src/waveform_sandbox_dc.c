#include "audio_spectrum_analyzer.h"
#include "raylib.h"
#include "rlgl.h"
#include <GL/gl.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

static const char* domain = "WAVEFORM-SANDBOX-DC";

static Wave wav = {0};
static AudioStream audio_stream = {0};
static size_t wav_cursor = 0;
static int16_t* wav_pcm16 = NULL;
static int16_t chunk_samples[WAVEFORM_AUDIO_STREAM_RING_BUFFER_SIZE] = {0};

static Vector3 waveform_vertices[WAVEFORM_BUFFER_SIZE] = {0};

static void update_waveform_vertices(float* audio_samples) {
    int sample_stride = WAVEFORM_WINDOW_SIZE / WAVEFORM_BUFFER_SIZE;
    float sample_column_width = (float)SCREEN_WIDTH / (float)WAVEFORM_BUFFER_SIZE;
    for (int sample_index = 0; sample_index < WAVEFORM_BUFFER_SIZE; sample_index++) {
        float sample_value = audio_samples[sample_index * sample_stride];
        float normalized_sample = 0.5f * (1.0f + sample_value);
        normalized_sample = (normalized_sample < 0.0f) ? 0.0f : (normalized_sample > 1.0f) ? 1.0f : normalized_sample;
        float sample_x = ((float)sample_index + 0.5f) * sample_column_width;
        float sample_y = floorf(normalized_sample * (float)(SCREEN_HEIGHT - 1) + 0.5f);
        waveform_vertices[sample_index].x = sample_x;
        waveform_vertices[sample_index].y = sample_y;
        waveform_vertices[sample_index].z = 0.0f;
    }
}

int main(void) {
    float audio_samples[WAVEFORM_WINDOW_SIZE] = {0};

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(WAVEFORM_AUDIO_STREAM_RING_BUFFER_SIZE);
    wav = LoadWave("/rd/shadertoy_experiment_22050hz_pcm16_mono.wav");
    WaveFormat(&wav, SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    audio_stream = LoadAudioStream(SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    PlayAudioStream(audio_stream);
    wav_pcm16 = (int16_t*)wav.data;
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        while (IsAudioStreamProcessed(audio_stream)) {
            for (int i = 0; i < WAVEFORM_AUDIO_STREAM_RING_BUFFER_SIZE; i++) {
                chunk_samples[i] = wav_pcm16[wav_cursor];
                if (++wav_cursor >= wav.frameCount) {
                    wav_cursor = 0;
                }
            }

            UpdateAudioStream(audio_stream, chunk_samples, WAVEFORM_AUDIO_STREAM_RING_BUFFER_SIZE);

            for (int i = 0; i < WAVEFORM_WINDOW_SIZE; i++) {
                audio_samples[i] = (float)chunk_samples[WAVEFORM_WINDOW_SIZE + i] / PCM_SAMPLE_MAX_F;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);
        rlSetLineWidth(LINE_WIDTH);
        update_waveform_vertices(audio_samples);
        rlEnableStatePointer(GL_VERTEX_ARRAY, waveform_vertices);

        // rlEnablePointMode();
        // rlSetPointSize(LINE_WIDTH);
        // rlDrawVertexArrayCustom(0, WAVEFORM_BUFFER_SIZE, GL_POINTS);
        //^^ BLACK SCREEN AND AN ERROR? "GL ERROR: GL_INVALID_VALUE when calling glEnable(GL_POINTS)""
        // rlDrawVertexArrayCustom(0, WAVEFORM_BUFFER_SIZE, GL_LINES);

        rlDrawVertexArrayCustom(0, WAVEFORM_BUFFER_SIZE, GL_LINE_STRIP);

        DrawFPS(540, 400);
        EndDrawing();
    }

    UnloadAudioStream(audio_stream);
    UnloadWave(wav);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
