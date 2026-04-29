#include "fffftt.h"

static const char* domain = "FFTW-GL11";

int main(void) {
    float fft_compute_ms = 0.0f;

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

    fftw_complex* fftw_input = fftw_malloc(sizeof(fftw_complex) * ANALYSIS_WINDOW_SIZE_IN_FRAMES);
    fftw_complex* fftw_output = fftw_malloc(sizeof(fftw_complex) * ANALYSIS_WINDOW_SIZE_IN_FRAMES);
    fftw_plan fftw_plan_state = fftw_create_plan(ANALYSIS_WINDOW_SIZE_IN_FRAMES, FFTW_FORWARD, FFTW_ESTIMATE);

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
        for (int i = 0; i < ANALYSIS_WINDOW_SIZE_IN_FRAMES; i++) {
            fftw_input[i].re = (fftw_real)fft_data.work_buffer[i].real;
            fftw_input[i].im = (fftw_real)fft_data.work_buffer[i].imaginary;
        }
        fftw_one(fftw_plan_state, fftw_input, fftw_output);
        for (int i = 0; i < ANALYSIS_WINDOW_SIZE_IN_FRAMES; i++) {
            fft_data.work_buffer[i].real = (float)fftw_output[i].re;
            fft_data.work_buffer[i].imaginary = (float)fftw_output[i].im;
        }
        fft_compute_ms = ((float)GetTime() - fft_start) * 1000.0f;

        build_spectrum();

        FFT_PROFILE_SAMPLE(fft_profile_data, domain, fft_compute_ms, (float)GetTime() - start_time, &fft_data);

        BeginDrawing();
        ClearBackground(BLACK);
        render_fft_frame();
        EndDrawing();
    }

    fftw_destroy_plan(fftw_plan_state);
    fftw_free(fftw_input);
    fftw_free(fftw_output);
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
