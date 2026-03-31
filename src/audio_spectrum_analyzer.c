#include "audio_spectrum_analyzer.h"
#include "raymath.h"
#include <math.h>

void apply_blackman_window(FFTData* fft_data, float* audio_samples) {
    for (int i = 0; i < FFT_WINDOW_SIZE; i++) {
        float x = (2.0f * PI * i) / (FFT_WINDOW_SIZE - 1.0f);
        float blackman_weight = BLACKMAN_A - BLACKMAN_B * cosf(x) + BLACKMAN_C * cosf(2.0f * x);
        float sample = audio_samples[i];

        fft_data->work_buffer[i].real = sample * blackman_weight;
        fft_data->work_buffer[i].imaginary = 0.0f;
    }
}

void apply_blackman_window_fftw_complex(fftw_complex* fft_input, float* audio_samples) {
    for (int i = 0; i < FFT_WINDOW_SIZE; i++) {
        float x = (2.0f * PI * i) / (FFT_WINDOW_SIZE - 1.0f);
        float blackman_weight = BLACKMAN_A - BLACKMAN_B * cosf(x) + BLACKMAN_C * cosf(2.0f * x);
        float sample = audio_samples[i];
        float windowed_sample = sample * blackman_weight;

        fft_input[i].re = (fftw_real)windowed_sample;
        fft_input[i].im = 0.0;
    }
}

void cooley_tukey_fft_slow(FFTComplex* spectrum) {
    int j = 0;

    for (int i = 1; i < FFT_WINDOW_SIZE - 1; i++) {
        int bit = FFT_WINDOW_SIZE >> 1;

        while (j >= bit) {
            j -= bit;
            bit >>= 1;
        }

        j += bit;
        if (i < j) {
            FFTComplex tmp = spectrum[i];
            spectrum[i] = spectrum[j];
            spectrum[j] = tmp;
        }
    }

    for (int len = 2; len <= FFT_WINDOW_SIZE; len <<= 1) {
        float angle_rad = -2.0f * PI / len;
        FFTComplex twiddle_unit = {cosf(angle_rad), sinf(angle_rad)};

        for (int i = 0; i < FFT_WINDOW_SIZE; i += len) {
            FFTComplex twiddle_cur = {1.0f, 0.0f};

            for (int k = 0; k < len / 2; k++) {
                FFTComplex even = spectrum[i + k];
                FFTComplex odd = spectrum[i + k + len / 2];
                FFTComplex twiddled_odd = {odd.real * twiddle_cur.real - odd.imaginary * twiddle_cur.imaginary,
                                           odd.real * twiddle_cur.imaginary + odd.imaginary * twiddle_cur.real};
                float twiddle_real_next =
                    twiddle_cur.real * twiddle_unit.real - twiddle_cur.imaginary * twiddle_unit.imaginary;

                spectrum[i + k].real = even.real + twiddled_odd.real;
                spectrum[i + k].imaginary = even.imaginary + twiddled_odd.imaginary;
                spectrum[i + k + len / 2].real = even.real - twiddled_odd.real;
                spectrum[i + k + len / 2].imaginary = even.imaginary - twiddled_odd.imaginary;
                twiddle_cur.imaginary =
                    twiddle_cur.real * twiddle_unit.imaginary + twiddle_cur.imaginary * twiddle_unit.real;
                twiddle_cur.real = twiddle_real_next;
            }
        }
    }
}

static void update_spectrum_bin(FFTData* fft_data, float* smoothed_spectrum, int bin, float real, float imaginary) {
    float linear_magnitude = sqrtf(real * real + imaginary * imaginary) / FFT_WINDOW_SIZE;
    float smoothed_magnitude =
        SMOOTHING_TIME_CONSTANT * fft_data->prev_magnitudes[bin] + (1.0f - SMOOTHING_TIME_CONSTANT) * linear_magnitude;
    float db = logf(fmaxf(smoothed_magnitude, MIN_LOG_MAGNITUDE)) * DB_TO_LINEAR_SCALE;
    float normalized = (db - MIN_DECIBELS) * INVERSE_DECIBEL_RANGE;
    float clamped_magnitude = Clamp(normalized, 0.0f, 1.0f);

    fft_data->prev_magnitudes[bin] = smoothed_magnitude;
    smoothed_spectrum[bin] = clamped_magnitude;
}

void clean_up_fft(FFTData* fft_data) {
    float* smoothed_spectrum = fft_data->fft_history[fft_data->history_pos];

    for (int bin = 0; bin < BUFFER_SIZE; bin++) {
        float re = fft_data->work_buffer[bin].real;
        float im = fft_data->work_buffer[bin].imaginary;
        update_spectrum_bin(fft_data, smoothed_spectrum, bin, re, im);
    }

    fft_data->history_pos = (fft_data->history_pos + 1) % FFT_HISTORY_FRAME_COUNT;
    fft_data->frame_index++;
}

void clean_up_fftw_complex(FFTData* fft_data, fftw_complex* fft_output) {
    float* smoothed_spectrum = fft_data->fft_history[fft_data->history_pos];

    for (int bin = 0; bin < BUFFER_SIZE; bin++) {
        float re = (float)fft_output[bin].re;
        float im = (float)fft_output[bin].im;
        update_spectrum_bin(fft_data, smoothed_spectrum, bin, re, im);
    }

    fft_data->history_pos = (fft_data->history_pos + 1) % FFT_HISTORY_FRAME_COUNT;
    fft_data->frame_index++;
}

void render_frame(FFTData* fft_data) {
    float frames_since_tapback = floorf(fft_data->tapback_pos / ((float)FFT_WINDOW_SIZE / EFFECTIVE_SAMPLE_RATE));
    frames_since_tapback = Clamp(frames_since_tapback, 0.0f, (float)(FFT_HISTORY_FRAME_COUNT - 1));
    int history_position =
        (fft_data->history_pos - 1 - (int)frames_since_tapback + FFT_HISTORY_FRAME_COUNT) % FFT_HISTORY_FRAME_COUNT;
    float cell_width = (float)SCREEN_WIDTH / (float)BUFFER_SIZE;

    for (int bin = 0; bin < BUFFER_SIZE; bin++) {
        int x_0 = (int)floorf((float)bin * cell_width);
        int x_1 = (int)ceilf((float)(bin + 1) * cell_width);
        int column_width = (x_1 - x_0) - 1;
        float amplitude = fft_data->fft_history[history_position][bin];

        if (column_width < MIN_SPECTRUM_COLUMN_WIDTH)
            column_width = MIN_SPECTRUM_COLUMN_WIDTH;
        if (amplitude <= 0.0f)
            continue;

        int column_height = (int)ceilf(amplitude * (float)SCREEN_HEIGHT);
        DrawRectangle(x_0, SCREEN_HEIGHT - column_height, column_width, column_height, WHITE);
    }
}
