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
                float twiddle_real_next = twiddle_cur.real * twiddle_unit.real - twiddle_cur.imaginary * twiddle_unit.imaginary;

                spectrum[i + k].real = even.real + twiddled_odd.real;
                spectrum[i + k].imaginary = even.imaginary + twiddled_odd.imaginary;
                spectrum[i + k + len / 2].real = even.real - twiddled_odd.real;
                spectrum[i + k + len / 2].imaginary = even.imaginary - twiddled_odd.imaginary;
                twiddle_cur.imaginary = twiddle_cur.real * twiddle_unit.imaginary + twiddle_cur.imaginary * twiddle_unit.real;
                twiddle_cur.real = twiddle_real_next;
            }
        }
    }
}

static void update_spectrum_bin(FFTData* fft_data, float* smoothed_spectrum, int bin, float real, float imaginary) {
    float linear_magnitude = sqrtf(real * real + imaginary * imaginary) / FFT_WINDOW_SIZE;
    float smoothed_magnitude = SMOOTHING_TIME_CONSTANT * fft_data->prev_magnitudes[bin] + (1.0f - SMOOTHING_TIME_CONSTANT) * linear_magnitude;
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

void render_fft_frame(FFTData* fft_data) {
    float frames_since_tapback = floorf(fft_data->tapback_pos / ((float)FFT_WINDOW_SIZE / EFFECTIVE_SAMPLE_RATE));
    frames_since_tapback = Clamp(frames_since_tapback, 0.0f, (float)(FFT_HISTORY_FRAME_COUNT - 1));
    int history_frame_index = (fft_data->history_pos - 1 - (int)frames_since_tapback + FFT_HISTORY_FRAME_COUNT) % FFT_HISTORY_FRAME_COUNT;
    float* fft_frame = fft_data->fft_history[history_frame_index];

    float cell_width = (float)SCREEN_WIDTH / (float)BUFFER_SIZE;    // fft.glsl#L19 float cellWidth = iResolution.x / NUM_OF_BINS;
    for (int bin_index = 0; bin_index < BUFFER_SIZE; bin_index++) { // fft.glsl#L20 float binIndex = floor(fragCoord.x / cellWidth);
        int bin_x_min = (int)floorf((float)bin_index * cell_width); //????? fft.glsl#L21 float localX = mod(fragCoord.x, cellWidth);
        int bin_x_max = (int)ceilf((float)(bin_index + 1) * cell_width);
        int bar_width = (bin_x_max - bin_x_min) - 1; // fft.glsl#L22 float barWidth = cellWidth - 1.0;

        if (bar_width < MIN_SPECTRUM_COLUMN_WIDTH) {
            bar_width = MIN_SPECTRUM_COLUMN_WIDTH;
        }

        float amplitude = fft_frame[bin_index]; // fft.glsl#L28 float amplitude = texture(iChannel0, sampleCoord).r;
        if (amplitude <= 0.0f) {
            continue;
        }

        int bar_y = (int)ceilf(amplitude * (float)SCREEN_HEIGHT); // fft.glsl#L29 float barY = 1.0 - fragTexCoord.y;
        // fft.glsl#L30 if (barY < amplitude) color = WHITE;
        DrawRectangle(bin_x_min, SCREEN_HEIGHT - bar_y, bar_width, bar_y, WHITE);
    }
}

void render_waveform_frame(float* audio_samples) {
    int sample_stride = WAVEFORM_WINDOW_SIZE / WAVEFORM_BUFFER_SIZE;
    float sample_column_width = (float)SCREEN_WIDTH / (float)WAVEFORM_BUFFER_SIZE;
    int line_thickness_px = (int)floorf(LINE_WIDTH + 0.5f); // waveform.glsl#L7 #define LINE_WIDTH 1.0
    // waveform.glsl#L12 float sample_index = floor(frag_coord.x / cell_width);
    for (int sample_index = 0; sample_index < WAVEFORM_BUFFER_SIZE; sample_index++) {
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
