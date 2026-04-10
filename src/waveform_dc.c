#include "fffftt.h"
#include "rlgl.h"
#include <GL/gl.h>

static const char* domain = "WAVEFORM-DC";

static Wave wav = {0};
static AudioStream audio_stream = {0};
static size_t wav_cursor = 0;
static int16_t* wav_pcm16 = NULL;
static int16_t chunk_samples[AUDIO_STREAM_RING_BUFFER_SIZE] = {0};
static Vector3 waveform_vertices[BUFFER_SIZE] = {0};

#define WAVEFORM_LINE_WIDTH 5.0f // waveform.glsl#L7 #define LINE_WIDTH 1.0

static void update_waveform_vertices(float* audio_samples) {
    int sample_stride = WINDOW_SIZE / BUFFER_SIZE;
    float sample_column_width = (float)SCREEN_WIDTH / (float)BUFFER_SIZE;
    for (int sample_index = 0; sample_index < BUFFER_SIZE; sample_index++) {
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

static void render_waveform_frame(float* audio_samples) {
    int sample_stride = WINDOW_SIZE / BUFFER_SIZE;
    float sample_column_width = (float)SCREEN_WIDTH / (float)BUFFER_SIZE;
    int line_thickness_px = (int)floorf(WAVEFORM_LINE_WIDTH + 0.5f); // waveform.glsl#L7 #define LINE_WIDTH 1.0

    // waveform.glsl#L12 float sample_index = floor(frag_coord.x / cell_width);
    for (int sample_index = 0; sample_index < BUFFER_SIZE; sample_index++) {
        int sample_x_min = (int)floorf((float)sample_index * sample_column_width);
        // waveform.glsl#L13 float sample_x = (sample_index + 0.5) / total_waveform_buffer_size_in_samples;
        int sample_x_max = (int)floorf((float)(sample_index + 1) * sample_column_width);

        if (sample_x_max <= sample_x_min) {
            sample_x_max = sample_x_min + 1;
        }

        float sample_value = audio_samples[sample_index * sample_stride];
        float normalized_sample = 0.5f * (1.0f + sample_value);
        normalized_sample = Clamp(normalized_sample, 0.0f, 1.0f);
        // waveform.glsl#L16 float line_y = floor(sample_value * (iResolution.y - 1.0) + 0.5);
        int line_y = (int)floorf(normalized_sample * (float)(SCREEN_HEIGHT - 1) + 0.5f);

        int line_y_min = line_y - (line_thickness_px - 1) / 2;
        int line_y_max = line_y_min + line_thickness_px;

        line_y_min = Clamp(line_y_min, 0, SCREEN_HEIGHT);
        line_y_max = Clamp(line_y_max, 0, SCREEN_HEIGHT);

        if (line_y_max > line_y_min) {
            // waveform.glsl#L19 vec4 color = mix(BLACK, WHITE, line);
            DrawRectangle(sample_x_min, line_y_min, sample_x_max - sample_x_min, line_y_max - line_y_min, WHITE);
        }
    }
}

int main(void) {
    float audio_samples[WINDOW_SIZE] = {0};

    SetTraceLogLevel(LOG_WARNING); // TODO: note this should be commented out for testing logs on
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_STREAM_RING_BUFFER_SIZE);
    wav = LoadWave(RD_SHADERTOY_EXPERIMENT_22K_WAV);
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
            for (int i = 0; i < AUDIO_STREAM_RING_BUFFER_SIZE; i++) {
                chunk_samples[i] = wav_pcm16[wav_cursor];
                if (++wav_cursor >= wav.frameCount) {
                    wav_cursor = 0;
                }
            }

            UpdateAudioStream(audio_stream, chunk_samples, AUDIO_STREAM_RING_BUFFER_SIZE);

            for (int i = 0; i < WINDOW_SIZE; i++) {
                audio_samples[i] = (float)chunk_samples[WINDOW_SIZE + i] / PCM_SAMPLE_MAX_F;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);
        // TODO: BELOW IS FOR EXPERIMENTING WITH DIFFERENT DRAW TYPES
        // render_waveform_frame(audio_samples); // TODO: pendantic shadertoy parity example
        update_waveform_vertices(audio_samples);
        rlEnableStatePointer(GL_VERTEX_ARRAY, waveform_vertices);
        rlSetLineWidth(WAVEFORM_LINE_WIDTH);
        rlDrawVertexArrayCustom(0, BUFFER_SIZE, GL_LINE_STRIP);
        // rlDrawVertexArrayCustom(0, BUFFER_SIZE, GL_LINES);
        // rlDrawVertexArrayCustom(0, BUFFER_SIZE, GL_TRIANGLES); //TODO: NOT YET TESTED
        // rlDrawVertexArrayCustom(0, BUFFER_SIZE, GL_TRIANGLE_STRIP);  //TODO: NOT YET TESTED

        // rlEnablePointMode();
        // rlSetPointSize(WAVEFORM_LINE_WIDTH);
        // rlDrawVertexArrayCustom(0, BUFFER_SIZE, GL_POINTS); //TODO: NOT YET TESTED
        EndDrawing();
    }

    UnloadAudioStream(audio_stream);
    UnloadWave(wav);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
