#include "fffftt.h"
#include "GL/gl.h"

static const char* domain = "WAVEFORM-DC";
static Vector3 waveform_vertices[ANALYSIS_WAVEFORM_SAMPLE_COUNT] = {0};

#define WAVEFORM_LINE_WIDTH 5.0f // waveform.glsl#L7 #define LINE_WIDTH 1.0

static void update_waveform_vertices(void) {
    int sample_stride = ANALYSIS_WINDOW_SIZE_IN_FRAMES / ANALYSIS_WAVEFORM_SAMPLE_COUNT;
    float sample_column_width = (float)SCREEN_WIDTH / (float)ANALYSIS_WAVEFORM_SAMPLE_COUNT;
    for (int sample_index = 0; sample_index < ANALYSIS_WAVEFORM_SAMPLE_COUNT; sample_index++) {
        float sample_value = analysis_window_samples[sample_index * sample_stride];
        float normalized_sample = 0.5f * (1.0f + sample_value);
        normalized_sample = (normalized_sample < 0.0f) ? 0.0f : (normalized_sample > 1.0f) ? 1.0f : normalized_sample;
        float sample_x = ((float)sample_index + 0.5f) * sample_column_width;
        float sample_y = floorf(normalized_sample * (float)(SCREEN_HEIGHT - 1) + 0.5f);
        waveform_vertices[sample_index].x = sample_x;
        waveform_vertices[sample_index].y = sample_y;
        waveform_vertices[sample_index].z = 0.0f;
    }
}

static void render_waveform_frame(void) {
    int sample_stride = ANALYSIS_WINDOW_SIZE_IN_FRAMES / ANALYSIS_WAVEFORM_SAMPLE_COUNT;
    float sample_column_width = (float)SCREEN_WIDTH / (float)ANALYSIS_WAVEFORM_SAMPLE_COUNT;
    int line_thickness_px = (int)floorf(WAVEFORM_LINE_WIDTH + 0.5f); // waveform.glsl#L7 #define LINE_WIDTH 1.0

    // waveform.glsl#L12 float sample_index = floor(frag_coord.x / cell_width);
    for (int sample_index = 0; sample_index < ANALYSIS_WAVEFORM_SAMPLE_COUNT; sample_index++) {
        int sample_x_min = (int)floorf((float)sample_index * sample_column_width);
        // waveform.glsl#L13 float sample_x = (sample_index + 0.5) / total_waveform_buffer_size_in_samples;
        int sample_x_max = (int)floorf((float)(sample_index + 1) * sample_column_width);

        if (sample_x_max <= sample_x_min) {
            sample_x_max = sample_x_min + 1;
        }

        float sample_value = analysis_window_samples[sample_index * sample_stride];
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
    SetTraceLogLevel(LOG_WARNING); // TODO: note this should be commented out for testing logs on
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
    wave = LoadWave(RD_SHADERTOY_EXPERIMENT_22K_WAV);
    WaveFormat(&wave, SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    audio_stream = LoadAudioStream(SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    PlayAudioStream(audio_stream);

    wave_pcm16 = (int16_t*)wave.data;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        while (IsAudioStreamProcessed(audio_stream)) {
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
        }

        BeginDrawing();
        ClearBackground(BLACK);
        // TODO: BELOW IS FOR EXPERIMENTING WITH DIFFERENT DRAW TYPES
        // render_waveform_frame(); // TODO: pendantic shadertoy parity example
        update_waveform_vertices();
        rlEnableStatePointer(GL_VERTEX_ARRAY, waveform_vertices);
        rlSetLineWidth(WAVEFORM_LINE_WIDTH);
        glDrawArrays(GL_LINE_STRIP, 0, ANALYSIS_WAVEFORM_SAMPLE_COUNT);
        // glDrawArrays(GL_LINES, 0, ANALYSIS_WAVEFORM_SAMPLE_COUNT);
        // glDrawArrays(GL_TRIANGLES, 0, ANALYSIS_WAVEFORM_SAMPLE_COUNT);
        // glDrawArrays(GL_TRIANGLE_STRIP, 0, ANALYSIS_WAVEFORM_SAMPLE_COUNT);

        // rlEnablePointMode();
        // rlSetPointSize(WAVEFORM_LINE_WIDTH);
        // glDrawArrays(GL_POINTS, 0, ANALYSIS_WAVEFORM_SAMPLE_COUNT);
        EndDrawing();
    }

    UnloadAudioStream(audio_stream);
    UnloadWave(wave);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
