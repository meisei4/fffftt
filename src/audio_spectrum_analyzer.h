#ifndef AUDIO_SPECTRUM_ANALYZER_H
#define AUDIO_SPECTRUM_ANALYZER_H

#include "raylib.h"
#include "../fftw_1997/fftw.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

#ifdef PLATFORM_DREAMCAST
#include <dc/perfctr.h>
#endif // PLATFORM_DREAMCAST

#define MONO 1
#define STEREO_CHANNEL_COUNT 2
#define SAMPLE_RATE 22050
#define FFT_WINDOW_SIZE 1024
#define BUFFER_SIZE 512
#define PER_SAMPLE_BIT_DEPTH 16
#define MILLISECONDS_PER_SECOND 1000
#define SECONDS_PER_MINUTE 60
#define MILLISECONDS_PER_MINUTE (MILLISECONDS_PER_SECOND * SECONDS_PER_MINUTE)
#define AUDIO_STREAM_RING_BUFFER_SIZE (FFT_WINDOW_SIZE * STEREO_CHANNEL_COUNT)
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define FFT_WINDOW_DURATION_MILLISECONDS ((FFT_WINDOW_SIZE * MILLISECONDS_PER_SECOND) / SAMPLE_RATE)
#define FFT_HISTORICAL_SMOOTHING_DUR 2000
#define FFT_HISTORY_FRAME_COUNT                                                                               \
    (((FFT_HISTORICAL_SMOOTHING_DUR + FFT_WINDOW_DURATION_MILLISECONDS - 1) /                               \
      FFT_WINDOW_DURATION_MILLISECONDS) +                                                                    \
     1)
#define MIN_DECIBELS (-100.0f)
#define MAX_DECIBELS (-30.0f)
#define INVERSE_DECIBEL_RANGE (1.0f / (MAX_DECIBELS - MIN_DECIBELS))
#define LN_10 2.302585092994046f
#define DB_TO_LINEAR_SCALE (20.0f / LN_10)
#define SMOOTHING_TIME_CONSTANT 0.8f
#define LOG_TIMESTAMP_SIZE 32
#define PCM_SAMPLE_MAX_F 32767.0f
#define EFFECTIVE_SAMPLE_RATE ((float)SAMPLE_RATE)
#define BLACKMAN_A 0.42f
#define BLACKMAN_B 0.5f
#define BLACKMAN_C 0.08f
#define MIN_LOG_MAGNITUDE 1e-40f
#define COLOR_CHANNEL_MAX 255
#define TAPBACK_POS_DEFAULT 0.01f
#define MIN_SPECTRUM_COLUMN_WIDTH 1

typedef struct FFTProfileData {
    int last_log_sec;
    int run_count;
    float dur_sum_ms;
    float dur_min_ms;
    float dur_max_ms;
} FFTProfileData;

#define FFT_PROFILE_INIT() ((FFTProfileData){-1, 0, 0.0f, 0.0f, 0.0f})
#define FFT_PROFILE_DEFINE(name) FFTProfileData name = FFT_PROFILE_INIT()

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

void apply_blackman_window(FFTData *fft_data, float *audio_samples);
void apply_blackman_window_fftw_complex(fftw_complex *fft_input, float *audio_samples);
void cooley_tukey_fft_slow(FFTComplex *spectrum);
void clean_up_fft(FFTData *fft_data);
void clean_up_fftw_complex(FFTData *fft_data, fftw_complex *fft_output);
void render_frame(FFTData *fft_data);

#endif // AUDIO_SPECTRUM_ANALYZER_H
