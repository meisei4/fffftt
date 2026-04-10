#ifndef FFFFTT_H
#define FFFFTT_H

#include "raylib.h"
#include "raymath.h"
#include "../fftw_1997/fftw.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifdef PLATFORM_DREAMCAST
#include <dc/perfctr.h>
#endif // PLATFORM_DREAMCAST

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define MONO 1
#define SAMPLE_RATE 22050
#define PER_SAMPLE_BIT_DEPTH 16
#define PCM_SAMPLE_MAX_F 32767.0f
#define AUDIO_STREAM_FRAME_COUNT 2
#define WINDOW_SIZE 1024
#define BUFFER_SIZE 512 
#define AUDIO_STREAM_RING_BUFFER_SIZE (WINDOW_SIZE * AUDIO_STREAM_FRAME_COUNT)

#define CAMERA_FOVY_MIN 0.1f
#define CAMERA_FOVY_MAX 6.0f
#define CAMERA_FOVY_VELOCITY 6.0f
#define CAMERA_ORBIT_VELOCITY 2.0f
#define CAMERA_PITCH_MIN 0.1f
#define CAMERA_PITCH_MAX 2.5f

#define MILLISECONDS_PER_SECOND 1000
#define SECONDS_PER_MINUTE 60
#define MILLISECONDS_PER_MINUTE (MILLISECONDS_PER_SECOND * SECONDS_PER_MINUTE)
#define LOG_TIMESTAMP_SIZE 32

#define RD_COUNTRY_22K_WAV "/rd/country_22050hz_pcm16_mono.wav"
#define RD_COUNTRY_STEREO_MP3 "/rd/country_44100hz_128kbps_stereo.mp3"
#define RES_COUNTRY_STEREO_44K_WAV "src/resources/country_44100hz_pcm16_stereo.wav"

#define RD_SHADERTOY_EXPERIMENT_22K_WAV "/rd/shadertoy_experiment_22050hz_pcm16_mono.wav"
#define RES_SHADERTOY_EXPERIMENT_22K_WAV "src/resources/shadertoy_experiment_22050hz_pcm16_mono.wav"
#define RD_SHADERTOY_ELECTRONEBULAE_ONE_FOURTH_22K_WAV "/rd/shadertoy_electronebulae_one_fourth_22050hz_pcm16_mono.wav"

#define SHADER_FFT "src/resources/fft.glsl"
#define SHADER_WAVEFORM "src/resources/waveform.glsl"
#define SHADER_SOUND_ENVELOPE_BUFFER_A "src/resources/sound_envelope_buffer_a.glsl"
#define SHADER_SOUND_ENVELOPE_IMAGE "src/resources/sound_envelope_image.glsl"

#define TAPBACK_POS_DEFAULT 0.01f
#define EFFECTIVE_SAMPLE_RATE ((float)SAMPLE_RATE)

#define BLACKMAN_A 0.42f
#define BLACKMAN_B 0.5f
#define BLACKMAN_C 0.08f

#define MIN_DECIBELS (-100.0f)
#define MAX_DECIBELS (-30.0f)
#define INVERSE_DECIBEL_RANGE (1.0f / (MAX_DECIBELS - MIN_DECIBELS))
#define LN_10 2.302585092994046f
#define DB_TO_LINEAR_SCALE (20.0f / LN_10)
#define SMOOTHING_TIME_CONSTANT 0.8f
#define MIN_LOG_MAGNITUDE 1e-40f

#define FFT_HISTORICAL_SMOOTHING_DUR 2000
#define FFT_WINDOW_DURATION_MILLISECONDS ((WINDOW_SIZE * MILLISECONDS_PER_SECOND) / SAMPLE_RATE)
#define FFT_HISTORY_FRAME_COUNT (((FFT_HISTORICAL_SMOOTHING_DUR + FFT_WINDOW_DURATION_MILLISECONDS - 1) / FFT_WINDOW_DURATION_MILLISECONDS) + 1)

#define COLOR_CHANNEL_MAX 255
#define MIN_SPECTRUM_COLUMN_WIDTH 1

typedef struct FFTComplex {
    float real;
    float imaginary;
} FFTComplex;

typedef struct FFTData {
    FFTComplex *work_buffer;
    float *prev_magnitudes;
    unsigned int frame_index;
    float (*fft_history)[BUFFER_SIZE];
    int history_pos;
    float tapback_pos;
} FFTData;

typedef struct FFTProfileData {
    int last_log_sec;
    int run_count;
    float dur_sum_ms;
    float dur_min_ms;
    float dur_max_ms;
} FFTProfileData;

#define FFT_PROFILE_INIT() ((FFTProfileData){-1, 0, 0.0f, 0.0f, 0.0f})
#define FFT_PROFILE_DEFINE(name) FFTProfileData name = FFT_PROFILE_INIT()
#define FFT_PROFILE_SAMPLE(profile_data, domain, fft_compute_ms, elapsed_s, fft_data) ((void)0)

#ifdef FFT_ENABLE_PROFILE
#undef FFT_PROFILE_SAMPLE
#define FFT_PROFILE_SAMPLE(profile_data, domain, fft_compute_ms, elapsed_s, fft_data)                        \
    do {                                                                                                      \
        FFTProfileData* fftprof_profile = &(profile_data);                                                    \
        const char* fftprof_domain = (domain);                                                                \
        FFTData* fftprof_fft_data = (fft_data);                                                               \
        float fftprof_compute_ms = (fft_compute_ms);                                                          \
        float fftprof_elapsed_s = (elapsed_s);                                                                \
                                                                                                              \
        fftprof_profile->run_count++;                                                                         \
        fftprof_profile->dur_sum_ms += fftprof_compute_ms;                                                    \
                                                                                                              \
        if (fftprof_profile->run_count == 1) {                                                                \
            fftprof_profile->dur_min_ms = fftprof_compute_ms;                                                 \
            fftprof_profile->dur_max_ms = fftprof_compute_ms;                                                 \
        } else {                                                                                              \
            fftprof_profile->dur_min_ms = fminf(fftprof_profile->dur_min_ms, fftprof_compute_ms);            \
            fftprof_profile->dur_max_ms = fmaxf(fftprof_profile->dur_max_ms, fftprof_compute_ms);            \
        }                                                                                                     \
                                                                                                              \
        int fftprof_elapsed_sec = (int)fftprof_elapsed_s;                                                     \
        if (fftprof_elapsed_sec != fftprof_profile->last_log_sec) {                                           \
            if (fftprof_domain != NULL && fftprof_fft_data != NULL) {                                         \
                char fftprof_timestamp[LOG_TIMESTAMP_SIZE];                                                   \
                int fftprof_elapsed_ms = (int)(fftprof_elapsed_s * (float)MILLISECONDS_PER_SECOND);          \
                int fftprof_elapsed_minutes = fftprof_elapsed_ms / MILLISECONDS_PER_MINUTE;                  \
                int fftprof_seconds = (fftprof_elapsed_ms / MILLISECONDS_PER_SECOND) % SECONDS_PER_MINUTE;  \
                int fftprof_millis = fftprof_elapsed_ms % MILLISECONDS_PER_SECOND;                           \
                float fftprof_avg_ms = fftprof_profile->dur_sum_ms / (float)fftprof_profile->run_count;      \
                                                                                                              \
                snprintf(fftprof_timestamp, sizeof(fftprof_timestamp), "%02d:%02d.%03d",                     \
                         fftprof_elapsed_minutes, fftprof_seconds, fftprof_millis);                          \
                TraceLog(LOG_INFO,                                                                             \
                         "[%s] [%s] frame=%u; fft_run_count=%d; fft_run_dur_min_ms=%.3f; "                   \
                         "fft_run_dur_max_ms=%.3f; fft_run_dur_avg_ms=%.3f",                                  \
                         fftprof_timestamp,                                                                    \
                         fftprof_domain,                                                                       \
                         fftprof_fft_data->frame_index,                                                        \
                         fftprof_profile->run_count,                                                           \
                         fftprof_profile->dur_min_ms,                                                          \
                         fftprof_profile->dur_max_ms,                                                          \
                         fftprof_avg_ms);                                                                      \
            }                                                                                                 \
                                                                                                              \
            fftprof_profile->last_log_sec = fftprof_elapsed_sec;                                              \
            fftprof_profile->run_count = 0;                                                                   \
            fftprof_profile->dur_sum_ms = 0.0f;                                                               \
        }                                                                                                     \
    } while (0)
#endif // FFT_ENABLE_PROFILE

static inline uint64_t time_nanoseconds(void) {
#ifdef PLATFORM_DREAMCAST
    return perf_cntr_timer_ns();
#else
    return (uint64_t)(GetTime() * 1000000000.0);
#endif // PLATFORM_DREAMCAST
}

static inline float elapsed_milliseconds(uint64_t start_ns) {
    return (float)(time_nanoseconds() - start_ns) * 0.000001f;
}

static inline void apply_blackman_window(FFTData* fft_data, float* audio_samples) {
    for (int i = 0; i < WINDOW_SIZE; i++) {
        float x = (2.0f * PI * i) / (WINDOW_SIZE - 1.0f);
        float blackman_weight = BLACKMAN_A - BLACKMAN_B * cosf(x) + BLACKMAN_C * cosf(2.0f * x);
        float sample = audio_samples[i];

        fft_data->work_buffer[i].real = sample * blackman_weight;
        fft_data->work_buffer[i].imaginary = 0.0f;
    }
}

static inline void apply_blackman_window_fftw_complex(fftw_complex* fft_input, float* audio_samples) {
    for (int i = 0; i < WINDOW_SIZE; i++) {
        float x = (2.0f * PI * i) / (WINDOW_SIZE - 1.0f);
        float blackman_weight = BLACKMAN_A - BLACKMAN_B * cosf(x) + BLACKMAN_C * cosf(2.0f * x);
        float sample = audio_samples[i];
        float windowed_sample = sample * blackman_weight;

        fft_input[i].re = (fftw_real)windowed_sample;
        fft_input[i].im = 0.0;
    }
}

static inline void cooley_tukey_fft_slow(FFTComplex* spectrum) {
    int j = 0;

    for (int i = 1; i < WINDOW_SIZE - 1; i++) {
        int bit = WINDOW_SIZE >> 1;

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

    for (int len = 2; len <= WINDOW_SIZE; len <<= 1) {
        float angle_rad = -2.0f * PI / len;
        FFTComplex twiddle_unit = {cosf(angle_rad), sinf(angle_rad)};

        for (int i = 0; i < WINDOW_SIZE; i += len) {
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

static inline void update_spectrum_bin(FFTData* fft_data, float* smoothed_spectrum, int bin, float real, float imaginary) {
    float linear_magnitude = sqrtf(real * real + imaginary * imaginary) / WINDOW_SIZE;
    float smoothed_magnitude = SMOOTHING_TIME_CONSTANT * fft_data->prev_magnitudes[bin] + (1.0f - SMOOTHING_TIME_CONSTANT) * linear_magnitude;
    float db = logf(fmaxf(smoothed_magnitude, MIN_LOG_MAGNITUDE)) * DB_TO_LINEAR_SCALE;
    float normalized = (db - MIN_DECIBELS) * INVERSE_DECIBEL_RANGE;
    float clamped_magnitude = Clamp(normalized, 0.0f, 1.0f);

    fft_data->prev_magnitudes[bin] = smoothed_magnitude;
    smoothed_spectrum[bin] = clamped_magnitude;
}

static inline void clean_up_fft(FFTData* fft_data) {
    float* smoothed_spectrum = fft_data->fft_history[fft_data->history_pos];

    for (int bin = 0; bin < BUFFER_SIZE; bin++) {
        float re = fft_data->work_buffer[bin].real;
        float im = fft_data->work_buffer[bin].imaginary;
        update_spectrum_bin(fft_data, smoothed_spectrum, bin, re, im);
    }

    fft_data->history_pos = (fft_data->history_pos + 1) % FFT_HISTORY_FRAME_COUNT;
    fft_data->frame_index++;
}

static inline void clean_up_fftw_complex(FFTData* fft_data, fftw_complex* fft_output) {
    float* smoothed_spectrum = fft_data->fft_history[fft_data->history_pos];

    for (int bin = 0; bin < BUFFER_SIZE; bin++) {
        float re = (float)fft_output[bin].re;
        float im = (float)fft_output[bin].im;
        update_spectrum_bin(fft_data, smoothed_spectrum, bin, re, im);
    }

    fft_data->history_pos = (fft_data->history_pos + 1) % FFT_HISTORY_FRAME_COUNT;
    fft_data->frame_index++;
}

static inline void render_fft_frame(FFTData* fft_data) {
    float frames_since_tapback = floorf(fft_data->tapback_pos / ((float)WINDOW_SIZE / EFFECTIVE_SAMPLE_RATE));
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

#endif // FFFFTT_H
