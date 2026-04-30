#include "fffftt.h"

static const char* domain = "COOL-GL33";

int main(void) {
    float fft_compute_ms = 0.0f;
    // SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    float start_time = (float)GetTime();

    fft_data.tapback_pos = ANALYSIS_TAPBACK_POS_DEFAULT;
    fft_data.work_buffer = RL_CALLOC(ANALYSIS_WINDOW_SIZE_IN_FRAMES, sizeof(FFTComplex));
    fft_data.prev_spectrum_bin_levels = RL_CALLOC(ANALYSIS_SPECTRUM_BIN_COUNT, sizeof(float));
    fft_data.raw_spectrum_history_levels = RL_CALLOC(ANALYSIS_FFT_HISTORY_FRAME_COUNT, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]));
    fft_data.spectrum_history_levels = RL_CALLOC(ANALYSIS_FFT_HISTORY_FRAME_COUNT, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]));

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
    wave = LoadWave(RES_COUNTRY_STEREO_44K_WAV);
    WaveFormat(&wave, SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    audio_stream = LoadAudioStream(SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    PlayAudioStream(audio_stream);

    wave_pcm16 = (int16_t*)wave.data;

    Image fft_image = GenImageColor(ANALYSIS_SPECTRUM_BIN_COUNT, 1, BLACK);
    Color* fft_pixels = (Color*)fft_image.data;
    Texture2D fft_texture = LoadTextureFromImage(fft_image);
    Shader shader = LoadShader(NULL, SHADER_FFT);
    int resolution_location = GetShaderLocation(shader, "iResolution");
    int channel_location = GetShaderLocation(shader, "iChannel0");
    Vector2 resolution = {(float)SCREEN_WIDTH, (float)SCREEN_HEIGHT};

    SetTextureFilter(fft_texture, TEXTURE_FILTER_POINT);
    SetShaderValue(shader, resolution_location, &resolution, SHADER_UNIFORM_VEC2);
    SetShaderValueTexture(shader, channel_location, fft_texture);
    SetTargetFPS(60);

    FFT_PROFILE_DEFINE(fft_profile_data);

    while (!WindowShouldClose()) {
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

        apply_blackman_window();
        float fft_start = (float)GetTime();
        cooley_tukey_fft_slow(fft_data.work_buffer);
        fft_compute_ms = ((float)GetTime() - fft_start) * 1000.0f;

        build_spectrum();

        FFT_PROFILE_SAMPLE(fft_profile_data, domain, fft_compute_ms, (float)GetTime() - start_time, &fft_data);

        float frames_since_tapback = floorf(fft_data.tapback_pos / ((float)ANALYSIS_WINDOW_SIZE_IN_FRAMES / (float)ANALYSIS_SAMPLE_RATE));
        frames_since_tapback = fminf(fmaxf(frames_since_tapback, 0.0f), (float)(ANALYSIS_FFT_HISTORY_FRAME_COUNT - 1));
        int history_position = (fft_data.history_pos - 1 - (int)frames_since_tapback + ANALYSIS_FFT_HISTORY_FRAME_COUNT) % ANALYSIS_FFT_HISTORY_FRAME_COUNT;

        for (int bin = 0; bin < ANALYSIS_SPECTRUM_BIN_COUNT; bin++) {
            float spectrum_level = fft_data.spectrum_history_levels[history_position][bin];
            float normalized_spectrum_level = fminf(fmaxf(roundf(spectrum_level * (float)DRAW_COLOR_CHANNEL_MAX), 0.0f), (float)DRAW_COLOR_CHANNEL_MAX);
            fft_pixels[bin] = (Color){(unsigned char)normalized_spectrum_level, 0, 0, DRAW_COLOR_CHANNEL_MAX};
        }
        UpdateTexture(fft_texture, fft_image.data);

        BeginDrawing();
        ClearBackground(BLACK);
        BeginShaderMode(shader);
        SetShaderValueTexture(shader, channel_location, fft_texture);
        DrawTexturePro(fft_texture,
                       (Rectangle){0.0f, 0.0f, (float)fft_texture.width, (float)fft_texture.height},
                       (Rectangle){0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT},
                       (Vector2){0},
                       0.0f,
                       WHITE);
        EndShaderMode();
        EndDrawing();
    }

    UnloadShader(shader);
    UnloadTexture(fft_texture);
    UnloadImage(fft_image);
    UnloadAudioStream(audio_stream);
    UnloadWave(wave);
    CloseAudioDevice();
    RL_FREE(fft_data.raw_spectrum_history_levels);
    RL_FREE(fft_data.spectrum_history_levels);
    RL_FREE(fft_data.prev_spectrum_bin_levels);
    RL_FREE(fft_data.work_buffer);
    CloseWindow();
    return 0;
}
