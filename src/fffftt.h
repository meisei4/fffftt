#ifndef FFFFTT_H
#define FFFFTT_H

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "../sh4zam/include/sh4zam/shz_sh4zam.h"
#include "../fftw_1997/fftw.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifdef PLATFORM_DREAMCAST
#include <dc/perfctr.h>
#endif // PLATFORM_DREAMCAST

//TODO: investigate orbital camera!!!!
#define SINF(x) shz_sinf((x))
#define COSF(x) shz_cosf((x))
#define TANF(x) shz_tanf((x))
#define SQRTF(x) shz_sqrtf((x))
#define ATAN2F(y, x) shz_atan2f((y), (x))
#define ACOSF(x) shz_acosf((x))
#define POWF(x, y) shz_powf((x), (y))
#define EXPF(x) shz_expf((x))
#define FLOORF(x) shz_floorf((x))
#define CEILF(x) shz_ceilf((x))
#define FMODF(x, y) shz_fmodf((x), (y))
#define LOGF(x) shz_logf((x))
#define FMAXF(x, y) shz_fmaxf((x), (y))
#define FABSF(x) shz_fabsf((x))
#define MEMSET(dst, value, size) memset((dst), (value), (size))
#define MEMCPY(dst, src, size) memcpy((dst), (src), (size)) //TODO: Colors... hmmm
#define MEMCPY4(dst, src, size) shz_memcpy4((dst), (src), (size))
#define CLAMP(x, min, max) shz_clampf((x), (min), (max))
#define LERP(a, b, t) shz_lerpf((a), (b), (t))

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define SUNFLOWER CLITERAL(Color){255, 204, 153, 255}  // #FFCC99FF
#define BAHAMA_BLUE CLITERAL(Color){0, 102, 153, 255}  // #006699FF
#define MARINER CLITERAL(Color){51, 102, 204, 255}     // #3366CCFF
#define NEON_CARROT CLITERAL(Color){255, 153, 51, 255} // #FF9933FF

// https://en.wikipedia.org/wiki/Wuxing_(Chinese_philosophy)#Music swatches: https://chinesecolor.xyz/
// TODO: Western antiquity concepts maybe https://en.wikipedia.org/wiki/Pythagorean_tuning, https://en.wikipedia.org/wiki/Diatonic_and_chromatic#Greek_genera
#define GONG_GAO3_HUANG2 CLITERAL(Color){255, 247, 153, 255}      // #FFF799
#define GONG_JIN1_HUANG2 CLITERAL(Color){255, 182, 30, 255}       // #FFB61E
#define SHANG_XIANG4_YA2_BAI2 CLITERAL(Color){255, 251, 240, 255} // #FFFBF0
#define SHANG_YIN2_BAI2 CLITERAL(Color){241, 242, 244, 255}       // #F1F2F4
#define JUE_CUI4_QING1 CLITERAL(Color){27, 167, 132, 255}         // #1BA784
#define JUE_BI4_QING1 CLITERAL(Color){59, 129, 140, 255}          // #3B818C
#define JUE_ZHAN4_LAN2 CLITERAL(Color){15, 89, 164, 255}          // #0F59A4
#define ZHI_DAN1_ZHU1 CLITERAL(Color){190, 0, 47, 255}            // #BE002F
#define ZHI_QIU1_HONG2 CLITERAL(Color){192, 44, 56, 255}          // #C02C38
#define YU_XUAN2_QING1 CLITERAL(Color){61, 59, 79, 255}           // #3D3B4F
#define YU_MU4_LAN2_ZI3 CLITERAL(Color){128, 109, 158, 255}       // #806D9E
#define RETURN_HU3_PO4 CLITERAL(Color){202, 105, 36, 255}         // #CA6924
#define WUXING_PITCH_CLASS_LUT                                                                                                                                 \
    {GONG_GAO3_HUANG2,                                                                                                                                         \
     GONG_JIN1_HUANG2,                                                                                                                                         \
     SHANG_XIANG4_YA2_BAI2,                                                                                                                                    \
     SHANG_YIN2_BAI2,                                                                                                                                          \
     JUE_CUI4_QING1,                                                                                                                                           \
     JUE_BI4_QING1,                                                                                                                                            \
     JUE_ZHAN4_LAN2,                                                                                                                                           \
     ZHI_DAN1_ZHU1,                                                                                                                                            \
     ZHI_QIU1_HONG2,                                                                                                                                           \
     YU_XUAN2_QING1,                                                                                                                                           \
     YU_MU4_LAN2_ZI3,                                                                                                                                          \
     RETURN_HU3_PO4}

#define SRC_CHANNELS 1
#define AUDIO_DEVICE_CHANNELS 2

#define SRC_SAMPLE_RATE 22050
#define AUDIO_DEVICE_SAMPLE_RATE 44100
#define ANALYSIS_SAMPLE_RATE 22050 //44100

#define SRC_BIT_DEPTH 16
#define AUDIO_DEVICE_BIT_DEPTH 16
#define ANALYSIS_PCM16_UPPER_BOUND 32767.0f

#define ANALYSIS_WINDOW_SIZE_IN_FRAMES 1024 //2048
#define ANALYSIS_SPECTRUM_BIN_COUNT 512
#define ANALYSIS_WAVEFORM_SAMPLE_COUNT 512

#ifndef AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES
#define AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES 2048
#endif

#define Y_AXIS (Vector3){0.0f, 1.0f, 0.0f}
#define DEFAULT_SCALE (Vector3){1.0f, 1.0f, 1.0f}

#define RD_COUNTRY_22K_WAV "/rd/country_22050hz_pcm16_mono.wav"
#define RD_COUNTRY_STEREO_MP3 "/rd/country_44100hz_128kbps_stereo.mp3"
#define RES_COUNTRY_STEREO_44K_WAV "src/resources/country_44100hz_pcm16_stereo.wav"

#define RD_SHADERTOY_EXPERIMENT_22K_WAV "/rd/shadertoy_experiment_22050hz_pcm16_mono.wav"
#define RES_SHADERTOY_EXPERIMENT_22K_WAV "src/resources/shadertoy_experiment_22050hz_pcm16_mono.wav"
#define RD_SHADERTOY_ELECTRONEBULAE_ONE_FOURTH_22K_WAV "/rd/shadertoy_electronebulae_one_fourth_22050hz_pcm16_mono.wav"

#define RD_DDS_FFM_22K_WAV "/rd/dds_ffm_22050hz_pcm16_mono.wav"
#define RD_DDS_FFM_FULL_22K_WAV "/rd/dds_ffm_full_22050hz_pcm16_mono.wav"

#define RD_RAMA_22K_WAV "/rd/rama_22050hz_pcm16_mono.wav"
#define RD_RAMA_FULL_22K_WAV "/rd/rama_full_22050hz_pcm16_mono.wav"

#define RD_CT_LOR_22K_WAV "/rd/ct_lor_22050hz_pcm16_mono.wav"
#define RD_CT_LOR_FULL_22K_WAV "/rd/ct_lor_full_22050hz_pcm16_mono.wav"

#define RD_AT_UNTITLED_22K_WAV "/rd/at_untitled_22050hz_pcm16_mono.wav"
#define RD_AT_UNTITLED_FULL_22K_WAV "/rd/at_untitled_full_22050hz_pcm16_mono.wav"

#define RD_RAMA_22K_WAV "/rd/rama_22050hz_pcm16_mono.wav"
#define RD_RAMA_FULL_22K_WAV "/rd/rama_full_22050hz_pcm16_mono.wav"

#define RD_TJ_SAYO_22K_WAV "/rd/tj_sayo_22050hz_pcm16_mono.wav"
#define RD_TJ_SAYO_FULL_22K_WAV "/rd/tj_sayo_full_22050hz_pcm16_mono.wav"

// #define RD_FONT "/rd/vga_rom_f16.fnt"
// #define RD_FONT "/rd/vga_rom_f16_1px_tight.fnt"
#define RD_FONT "/rd/vga_rom_f16_0px_TIGHT.fnt" //TODO: nice 1KB...
#define FONT_SIZE 16.0f

#define SHADER_FFT "src/resources/fft.glsl"
#define SHADER_WAVEFORM "src/resources/waveform.glsl"
#define SHADER_SOUND_ENVELOPE_BUFFER_A "src/resources/sound_envelope_buffer_a.glsl"
#define SHADER_SOUND_ENVELOPE_IMAGE "src/resources/sound_envelope_image.glsl"

typedef struct FFTComplex {
    float real;
    float imaginary;
} FFTComplex;

typedef struct FFTData {
    FFTComplex* work_buffer;
    float* prev_spectrum_bin_levels;
    unsigned int frame_index;
    float (*raw_spectrum_history_levels)[ANALYSIS_SPECTRUM_BIN_COUNT];
    float (*spectrum_history_levels)[ANALYSIS_SPECTRUM_BIN_COUNT];
    int history_pos;
    float tapback_pos;
} FFTData;

#define MILLISECONDS_PER_SECOND 1000
#define SECONDS_PER_MINUTE 60
#define MILLISECONDS_PER_MINUTE (MILLISECONDS_PER_SECOND * SECONDS_PER_MINUTE)
#define LOG_TIMESTAMP_SIZE 32
#define SAMPLE_CURSOR_TO_MS(sample_cursor) (((sample_cursor) * MILLISECONDS_PER_SECOND) / SRC_SAMPLE_RATE)
#define MMSSMMM_FMT "%02d:%02d.%03d"
#define MMSSMMM_ARGS(elapsed_ms)                                                                                                                               \
    (elapsed_ms) / MILLISECONDS_PER_MINUTE, ((elapsed_ms) / MILLISECONDS_PER_SECOND) % SECONDS_PER_MINUTE, (elapsed_ms) % MILLISECONDS_PER_SECOND
#define FORMAT_MMSSMMM(timestamp, elapsed_ms)                                                                                                                  \
    do {                                                                                                                                                       \
        strncpy((timestamp), TextFormat(MMSSMMM_FMT, MMSSMMM_ARGS(elapsed_ms)), sizeof(timestamp) - 1);                                                        \
        (timestamp)[sizeof(timestamp) - 1] = '\0';                                                                                                             \
    } while (0)

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
#define FFT_PROFILE_SAMPLE(profile_data, domain, fft_compute_ms, elapsed_s, fft_data)                                                                          \
    do {                                                                                                                                                       \
        FFTProfileData* fftprof_profile = &(profile_data);                                                                                                     \
        const char* fftprof_domain = (domain);                                                                                                                 \
        FFTData* fftprof_fft_data = (fft_data);                                                                                                                \
        float fftprof_compute_ms = (fft_compute_ms);                                                                                                           \
        float fftprof_elapsed_s = (elapsed_s);                                                                                                                 \
                                                                                                                                                               \
        fftprof_profile->run_count++;                                                                                                                          \
        fftprof_profile->dur_sum_ms += fftprof_compute_ms;                                                                                                     \
                                                                                                                                                               \
        if (fftprof_profile->run_count == 1) {                                                                                                                 \
            fftprof_profile->dur_min_ms = fftprof_compute_ms;                                                                                                  \
            fftprof_profile->dur_max_ms = fftprof_compute_ms;                                                                                                  \
        } else {                                                                                                                                               \
            fftprof_profile->dur_min_ms = fminf(fftprof_profile->dur_min_ms, fftprof_compute_ms);                                                              \
            fftprof_profile->dur_max_ms = fmaxf(fftprof_profile->dur_max_ms, fftprof_compute_ms);                                                              \
        }                                                                                                                                                      \
                                                                                                                                                               \
        int fftprof_elapsed_sec = (int)fftprof_elapsed_s;                                                                                                      \
        if (fftprof_elapsed_sec != fftprof_profile->last_log_sec) {                                                                                            \
            if (fftprof_domain != NULL && fftprof_fft_data != NULL) {                                                                                          \
                char fftprof_timestamp[LOG_TIMESTAMP_SIZE];                                                                                                    \
                int fftprof_elapsed_ms = (int)(fftprof_elapsed_s * (float)MILLISECONDS_PER_SECOND);                                                            \
                float fftprof_avg_ms = fftprof_profile->dur_sum_ms / (float)fftprof_profile->run_count;                                                        \
                                                                                                                                                               \
                FORMAT_MMSSMMM(fftprof_timestamp, fftprof_elapsed_ms);                                                                                         \
                TraceLog(LOG_INFO,                                                                                                                             \
                         "[%s] [%s] frame=%u; fft_run_count=%d; fft_run_dur_min_ms=%.3f; "                                                                     \
                         "fft_run_dur_max_ms=%.3f; fft_run_dur_avg_ms=%.3f",                                                                                   \
                         fftprof_timestamp,                                                                                                                    \
                         fftprof_domain,                                                                                                                       \
                         fftprof_fft_data->frame_index,                                                                                                        \
                         fftprof_profile->run_count,                                                                                                           \
                         fftprof_profile->dur_min_ms,                                                                                                          \
                         fftprof_profile->dur_max_ms,                                                                                                          \
                         fftprof_avg_ms);                                                                                                                      \
            }                                                                                                                                                  \
                                                                                                                                                               \
            fftprof_profile->last_log_sec = fftprof_elapsed_sec;                                                                                               \
            fftprof_profile->run_count = 0;                                                                                                                    \
            fftprof_profile->dur_sum_ms = 0.0f;                                                                                                                \
        }                                                                                                                                                      \
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

static AudioStream audio_stream = {0};
static Wave wave = {0};
static int wave_cursor = 0;
static int16_t* wave_pcm16 = NULL;
static FFTData fft_data = {0};
static float analysis_window_samples[ANALYSIS_WINDOW_SIZE_IN_FRAMES] = {0};
// TODO: current stupid trick for draining my understanding of the worst case scenario: mid-stream pause/resume of raylib/miniaudio stream + AICA buffer
#define MAX_DRAIN_CHUNK_COUNT 3
static int16_t drain_chunk_samples[AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES] = {0};
static int16_t resume_chunk_samples[AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES] = {0};
static int16_t chunk_samples[AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES] = {0};
static int seek_delta_chunks = 0;
static int paused_wave_cursor = 0;
static bool is_paused = false;

#define ANALYSIS_TAPBACK_POS_DEFAULT 0.01f

#define ANALYSIS_BLACKMAN_A 0.42f
#define ANALYSIS_BLACKMAN_B 0.5f
#define ANALYSIS_BLACKMAN_C 0.08f

#define ANALYSIS_MIN_DECIBELS (-100.0f)
#define ANALYSIS_MAX_DECIBELS (-30.0f)
#define ANALYSIS_INVERSE_DECIBEL_RANGE (1.0f / (ANALYSIS_MAX_DECIBELS - ANALYSIS_MIN_DECIBELS))
#define LN_10 2.302585092994046f
#define ANALYSIS_DB_TO_LINEAR_SCALE (20.0f / LN_10)
#define ANALYSIS_SMOOTHING_TIME_CONSTANT 0.8f
#define ANALYSIS_MIN_LOG_MAGNITUDE 1e-40f

#define ANALYSIS_FFT_HISTORICAL_SMOOTHING_DUR 2000
#define ANALYSIS_WINDOW_DURATION_MS ((ANALYSIS_WINDOW_SIZE_IN_FRAMES * MILLISECONDS_PER_SECOND) / ANALYSIS_SAMPLE_RATE)
#define ANALYSIS_FFT_HISTORY_FRAME_COUNT (((ANALYSIS_FFT_HISTORICAL_SMOOTHING_DUR + ANALYSIS_WINDOW_DURATION_MS - 1) / ANALYSIS_WINDOW_DURATION_MS) + 1)

#define DRAW_MIN_SPECTRUM_BIN_WIDTH 1

static inline void apply_blackman_window(void) {
    for (int i = 0; i < ANALYSIS_WINDOW_SIZE_IN_FRAMES; i++) {
        float x = (2.0f * PI * i) / (ANALYSIS_WINDOW_SIZE_IN_FRAMES - 1.0f);
        float blackman_weight = ANALYSIS_BLACKMAN_A - ANALYSIS_BLACKMAN_B * cosf(x) + ANALYSIS_BLACKMAN_C * cosf(2.0f * x);
        float sample = analysis_window_samples[i];

        fft_data.work_buffer[i].real = sample * blackman_weight;
        fft_data.work_buffer[i].imaginary = 0.0f;
    }
}

static inline void apply_blackman_window_fftw_complex(fftw_complex* fft_input) {
    for (int i = 0; i < ANALYSIS_WINDOW_SIZE_IN_FRAMES; i++) {
        float x = (2.0f * PI * i) / (ANALYSIS_WINDOW_SIZE_IN_FRAMES - 1.0f);
        float blackman_weight = ANALYSIS_BLACKMAN_A - ANALYSIS_BLACKMAN_B * cosf(x) + ANALYSIS_BLACKMAN_C * cosf(2.0f * x);
        float sample = analysis_window_samples[i];
        float windowed_sample = sample * blackman_weight;

        fft_input[i].re = (fftw_real)windowed_sample;
        fft_input[i].im = 0.0;
    }
}

static inline void cooley_tukey_fft_slow(FFTComplex* spectrum) {
    int j = 0;

    for (int i = 1; i < ANALYSIS_WINDOW_SIZE_IN_FRAMES - 1; i++) {
        int bit = ANALYSIS_WINDOW_SIZE_IN_FRAMES >> 1;

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

    for (int len = 2; len <= ANALYSIS_WINDOW_SIZE_IN_FRAMES; len <<= 1) {
        float angle_rad = -2.0f * PI / len;
        FFTComplex twiddle_unit = {cosf(angle_rad), sinf(angle_rad)};

        for (int i = 0; i < ANALYSIS_WINDOW_SIZE_IN_FRAMES; i += len) {
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

static inline void update_spectrum_bin(float* smoothed_spectrum, int bin, float real, float imaginary) {
    float linear_magnitude = sqrtf(real * real + imaginary * imaginary) / ANALYSIS_WINDOW_SIZE_IN_FRAMES;
    float smoothed_magnitude =
        ANALYSIS_SMOOTHING_TIME_CONSTANT * fft_data.prev_spectrum_bin_levels[bin] + (1.0f - ANALYSIS_SMOOTHING_TIME_CONSTANT) * linear_magnitude;
    float db = logf(fmaxf(smoothed_magnitude, ANALYSIS_MIN_LOG_MAGNITUDE)) * ANALYSIS_DB_TO_LINEAR_SCALE;
    float normalized = (db - ANALYSIS_MIN_DECIBELS) * ANALYSIS_INVERSE_DECIBEL_RANGE;
    float clamped_magnitude = Clamp(normalized, 0.0f, 1.0f);

    fft_data.prev_spectrum_bin_levels[bin] = smoothed_magnitude;
    smoothed_spectrum[bin] = clamped_magnitude;
}

static inline void build_spectrum(void) {
    float* smoothed_spectrum = fft_data.spectrum_history_levels[fft_data.history_pos];
    float* raw_spectrum = fft_data.raw_spectrum_history_levels[fft_data.history_pos];

    for (int bin = 0; bin < ANALYSIS_SPECTRUM_BIN_COUNT; bin++) {
        float re = fft_data.work_buffer[bin].real;
        float im = fft_data.work_buffer[bin].imaginary;
        float linear_magnitude = sqrtf(re * re + im * im) / ANALYSIS_WINDOW_SIZE_IN_FRAMES;
        raw_spectrum[bin] = linear_magnitude;
        update_spectrum_bin(smoothed_spectrum, bin, re, im);
    }

    fft_data.history_pos = (fft_data.history_pos + 1) % ANALYSIS_FFT_HISTORY_FRAME_COUNT;
    fft_data.frame_index++;
}

static inline void build_spectrum_fftw(fftw_complex* fft_output) {
    float* smoothed_spectrum = fft_data.spectrum_history_levels[fft_data.history_pos];
    float* raw_spectrum = fft_data.raw_spectrum_history_levels[fft_data.history_pos];

    for (int bin = 0; bin < ANALYSIS_SPECTRUM_BIN_COUNT; bin++) {
        float re = (float)fft_output[bin].re;
        float im = (float)fft_output[bin].im;
        float linear_magnitude = sqrtf(re * re + im * im) / ANALYSIS_WINDOW_SIZE_IN_FRAMES;
        raw_spectrum[bin] = linear_magnitude;
        update_spectrum_bin(smoothed_spectrum, bin, re, im);
    }

    fft_data.history_pos = (fft_data.history_pos + 1) % ANALYSIS_FFT_HISTORY_FRAME_COUNT;
    fft_data.frame_index++;
}

static inline void render_fft_frame(void) {
    float frames_since_tapback = floorf(fft_data.tapback_pos / ((float)ANALYSIS_WINDOW_SIZE_IN_FRAMES / (float)ANALYSIS_SAMPLE_RATE));
    frames_since_tapback = Clamp(frames_since_tapback, 0.0f, (float)(ANALYSIS_FFT_HISTORY_FRAME_COUNT - 1));
    int history_frame_index = (fft_data.history_pos - 1 - (int)frames_since_tapback + ANALYSIS_FFT_HISTORY_FRAME_COUNT) % ANALYSIS_FFT_HISTORY_FRAME_COUNT;
    float* spectrum_bin_levels = fft_data.spectrum_history_levels[history_frame_index];

    float cell_width = (float)SCREEN_WIDTH / (float)ANALYSIS_SPECTRUM_BIN_COUNT;    // fft.glsl#L19 float cellWidth = iResolution.x / NUM_OF_BINS;
    for (int bin_index = 0; bin_index < ANALYSIS_SPECTRUM_BIN_COUNT; bin_index++) { // fft.glsl#L20 float binIndex = floor(fragCoord.x / cellWidth);
        int bin_x_min = (int)floorf((float)bin_index * cell_width);                 //????? fft.glsl#L21 float localX = mod(fragCoord.x, cellWidth);
        int bin_x_max = (int)ceilf((float)(bin_index + 1) * cell_width);
        int bar_width = (bin_x_max - bin_x_min) - 1; // fft.glsl#L22 float barWidth = cellWidth - 1.0;

        if (bar_width < DRAW_MIN_SPECTRUM_BIN_WIDTH) {
            bar_width = DRAW_MIN_SPECTRUM_BIN_WIDTH;
        }

        float spectrum_level = spectrum_bin_levels[bin_index]; // fft.glsl#L28 float amplitude = texture(iChannel0, sampleCoord).r;
        if (spectrum_level <= 0.0f) {
            continue;
        }

        int bar_y = (int)ceilf(spectrum_level * (float)SCREEN_HEIGHT); // fft.glsl#L29 float barY = 1.0 - fragTexCoord.y;
        // fft.glsl#L30 if (barY < amplitude) color = WHITE;
        DrawRectangle(bin_x_min, SCREEN_HEIGHT - bar_y, bar_width, bar_y, WHITE);
    }
}

// ENVELOPE AND TERRIAN PROFILES + CONSTANTS
#if defined(FFFFTT_PROFILE_SOUND_ENVELOPE_3D)
#define LANE_COUNT 5
#define LANE_POINT_COUNT 64
#define AMPLITUDE_Y_SCALE 2.0f
#define LINE_LENGTH_SCALE 1.75f
#elif defined(FFFFTT_PROFILE_SOUND_ENVELOPE_ISO)
#define LANE_COUNT 5
#define LANE_POINT_COUNT 64
#define AMPLITUDE_Y_SCALE 120.0f
#define LINE_LENGTH_SCALE 4.0f
#elif defined(FFFFTT_PROFILE_WAVEFORM_TERRAIN_3D)
#define LANE_COUNT 11
#define LANE_POINT_COUNT 33
// #define AMPLITUDE_Y_SCALE 2.0f
#define AMPLITUDE_Y_SCALE 1.0f
#define LINE_LENGTH_SCALE 5.0f
#elif defined(FFFFTT_PROFILE_FFT_TERRAIN_3D)
#define LANE_COUNT 11
#define LANE_POINT_COUNT 33
#define AMPLITUDE_Y_SCALE 3.0f
#define LINE_LENGTH_SCALE 5.0f
#else
#define LANE_COUNT 11
#define LANE_POINT_COUNT 33
#define AMPLITUDE_Y_SCALE 3.0f
#define LINE_LENGTH_SCALE 5.0f
#endif

#define HALF_SPAN 0.5f
#define FRONT_LANE_SMOOTHING 0.4f
#define WAVEFORM_SAMPLES_PER_LANE_POINT (ANALYSIS_WINDOW_SIZE_IN_FRAMES / LANE_POINT_COUNT)
#define ISOMETRIC_LANE_SPACING 9.0f
#define ISOMETRIC_ZOOM 3.0f
#define ISOMETRIC_GRID_CENTER_X (0.5f * (-(float)(LANE_COUNT - 1) * ISOMETRIC_LANE_SPACING + (float)(LANE_POINT_COUNT - 1)))
#define ISOMETRIC_GRID_CENTER_Y (0.25f * ((float)(LANE_POINT_COUNT - 1) + (float)(LANE_COUNT - 1) * ISOMETRIC_LANE_SPACING))
#define LANE_SPACING_SCALE 0.50f
#define POINT_SIZE_RASTER_PIXELS 3.0f
#define MESH_VERTEX_COUNT (LANE_COUNT * LANE_POINT_COUNT)
#define LINE_SEGMENT_COUNT (LANE_COUNT * (LANE_POINT_COUNT - 1))
#define LINE_INDEX_COUNT (LINE_SEGMENT_COUNT * 2)
#define TERRAIN_TRIANGLE_COUNT (((LANE_COUNT - 1) * (LANE_POINT_COUNT - 1)) * 2)
#define FLAT_VERTEX_COUNT (TERRAIN_TRIANGLE_COUNT * 3)
#define DRAW_COLOR_CHANNEL_MAX 255

#define TOP (Vector3){0.0f, 2.0f, 0.0f}
#define MIDDLE (Vector3){0.0f}
#define BOTTOM (Vector3){0.0f, -1.0f, 0.0f}
#define BOTTOM_BASS (Vector3){0.0f, -3.0f, 0.0f}

static float lane_point_values[LANE_COUNT][LANE_POINT_COUNT] = {0};

static inline void advance_lane_history(float* lane_point_values, int point_count) {
    for (int i = LANE_COUNT - 1; i > 0; i--) {
        for (int j = 0; j < point_count; j++) {
            lane_point_values[i * point_count + j] = lane_point_values[(i - 1) * point_count + j];
        }
    }
}

static inline void advance_lane_history_u8(unsigned char* lane_point_values, int point_count) {
    for (int i = LANE_COUNT - 1; i > 0; i--) {
        for (int j = 0; j < point_count; j++) {
            lane_point_values[i * point_count + j] = lane_point_values[(i - 1) * point_count + j];
        }
    }
}

static inline void smooth_front_lane(void) {
    for (int i = 0; i < LANE_POINT_COUNT; i++) {
        float sample_sum = 0.0f;
        int k = i * WAVEFORM_SAMPLES_PER_LANE_POINT;
        for (int j = 0; j < WAVEFORM_SAMPLES_PER_LANE_POINT; j++) {
            sample_sum += FABSF(analysis_window_samples[k + j]);
        }

        lane_point_values[0][i] = (sample_sum / (float)WAVEFORM_SAMPLES_PER_LANE_POINT) * FRONT_LANE_SMOOTHING;
    }
}

static inline void update_envelope_mesh_vertices_isometric(Vector3* vertices) {
    for (int i = 0; i < LANE_COUNT; i++) {
        float lane_offset = (float)i * ISOMETRIC_LANE_SPACING;
        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            int k = i * LANE_POINT_COUNT + j;
            float grid_x = (float)j - lane_offset;
            float grid_y = 0.5f * ((float)j + lane_offset) - lane_point_values[i][j] * AMPLITUDE_Y_SCALE;
            vertices[k].x = 0.5f * (float)SCREEN_WIDTH + (grid_x - ISOMETRIC_GRID_CENTER_X) * ISOMETRIC_ZOOM;
            vertices[k].y = 0.5f * (float)SCREEN_HEIGHT - (grid_y - ISOMETRIC_GRID_CENTER_Y) * ISOMETRIC_ZOOM;
            vertices[k].z = 0.0f;
        }
    }
}

static inline void update_envelope_mesh_vertices(Vector3* vertices) {
    for (int i = 0; i < LANE_COUNT; i++) {
        float z = HALF_SPAN - ((float)i / (float)(LANE_COUNT - 1));
        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            int k = i * LANE_POINT_COUNT + j;
            float x = (((float)j / (float)(LANE_POINT_COUNT - 1)) - HALF_SPAN) * LINE_LENGTH_SCALE;
            vertices[k].x = x;
            vertices[k].y = lane_point_values[i][j] * AMPLITUDE_Y_SCALE;
            vertices[k].z = z;
        }
    }
}

static inline void fill_mesh_indices_lane_topology(unsigned short* indices) {
    int k = 0;
    for (int i = 0; i < LANE_COUNT; i++) {
        for (int j = 0; j < LANE_POINT_COUNT - 1; j++) {
            unsigned short i_0 = (unsigned short)(i * LANE_POINT_COUNT + j);
            unsigned short i_1 = (unsigned short)(i_0 + 1);
            indices[k++] = i_0;
            indices[k++] = i_1;
        }
    }
}

static inline void fill_mesh_colors(Color* colors, int point_count) {
    for (int i = 0; i < LANE_COUNT; i++) {
        float v = (float)i / (float)(LANE_COUNT - 1);
        for (int j = 0; j < point_count; j++) {
            float u = (float)j / (float)(point_count - 1);
            int k = i * point_count + j;
            float r = (1.0f - u) * (1.0f - v) * MAGENTA.r + u * (1.0f - v) * BLUE.r + (1.0f - u) * v * RED.r + u * v * YELLOW.r;
            float g = (1.0f - u) * (1.0f - v) * MAGENTA.g + u * (1.0f - v) * BLUE.g + (1.0f - u) * v * RED.g + u * v * YELLOW.g;
            float b = (1.0f - u) * (1.0f - v) * MAGENTA.b + u * (1.0f - v) * BLUE.b + (1.0f - u) * v * RED.b + u * v * YELLOW.b;
            colors[k] = (Color){(unsigned char)r, (unsigned char)g, (unsigned char)b, DRAW_COLOR_CHANNEL_MAX};
        }
    }
}

static inline void update_mesh_vertices(float* vertices, const float* lane_point_values, int point_count) {
    for (int i = 0; i < LANE_COUNT; i++) {
        float lane_offset = ((float)i - 0.5f * (float)(LANE_COUNT - 1)) * LANE_SPACING_SCALE;
        for (int j = 0; j < point_count; j++) {
            int k = (i * point_count + j) * 3;
            float x = (((float)j / (float)(point_count - 1)) - HALF_SPAN) * LINE_LENGTH_SCALE;
            float y = lane_point_values[i * point_count + j] * AMPLITUDE_Y_SCALE;
            vertices[k + 0] = x;
            vertices[k + 1] = y;
            vertices[k + 2] = lane_offset; // NOTE: +Z orientation determined by raylib GenMeshPlane function!!
        }
    }
}

static void update_mesh_normals_smooth(float* normals, const float* vertices, int point_count) {
    for (int i = 0; i < LANE_COUNT; i++) {
        int i_prev = (i > 0) ? i - 1 : i;
        int i_next = (i < LANE_COUNT - 1) ? i + 1 : i;

        for (int j = 0; j < point_count; j++) {
            int j_prev = (j > 0) ? j - 1 : j;
            int j_next = (j < point_count - 1) ? j + 1 : j;

            int k_left = (i * point_count + j_prev) * 3;
            int k_right = (i * point_count + j_next) * 3;
            int k_back = (i_prev * point_count + j) * 3;
            int k_front = (i_next * point_count + j) * 3;
            int k_out = (i * point_count + j) * 3;

            //TODO: is there a more standarized way to do this? like can we anticipate tangents in dynamic vertex attributes or something???
            float tangent_x_x = vertices[k_right + 0] - vertices[k_left + 0];
            float tangent_x_y = vertices[k_right + 1] - vertices[k_left + 1];
            float tangent_z_y = vertices[k_front + 1] - vertices[k_back + 1];
            float tangent_z_z = vertices[k_front + 2] - vertices[k_back + 2];
            Vector3 n = Vector3Normalize((Vector3){-tangent_z_z * tangent_x_y, tangent_z_z * tangent_x_x, -tangent_z_y * tangent_x_x});

            normals[k_out + 0] = n.x;
            normals[k_out + 1] = n.y;
            normals[k_out + 2] = n.z;
        }
    }
}

static inline void update_mesh_texcoords_smooth_scroll(int w, int h, float* texcoords, int point_count, int texels_per_quad, float time) {
    const Vector2 scroll_direction = {1.0f, 0.25f}; //TODO: make this configurable and cleaner
    const float scroll_speed = 0.0125f;
    float scroll_s = FMODF(scroll_direction.x * scroll_speed * time, 1.0f);
    float scroll_t = FMODF(scroll_direction.y * scroll_speed * time, 1.0f);
    for (int i = 0; i < LANE_COUNT; i++) {
        for (int j = 0; j < point_count; j++) {
            int k = (i * point_count + j) * 2;
            float s = ((float)(j * texels_per_quad) / (float)w) + scroll_s;
            float t = ((float)(i * texels_per_quad) / (float)h) + scroll_t;
            texcoords[k + 0] = s;
            texcoords[k + 1] = t;
        }
    }
}

static void update_mesh_vertices_flat(float* dst_vertices, const float* src_vertices, const unsigned short* indices) {
    for (int i = 0; i < FLAT_VERTEX_COUNT; i++) {
        int src_index = indices[i];
        int src_vertex = src_index * 3;
        int dst_vertex = i * 3;
        dst_vertices[dst_vertex + 0] = src_vertices[src_vertex + 0];
        dst_vertices[dst_vertex + 1] = src_vertices[src_vertex + 1];
        dst_vertices[dst_vertex + 2] = src_vertices[src_vertex + 2];
    }
}

static void expand_mesh_colors_flat(Color* dst_colors, const Color* src_colors, const unsigned short* indices) {
    for (int i = 0; i < FLAT_VERTEX_COUNT; i++) {
        int src_index = indices[i];
        dst_colors[i] = src_colors[src_index];
    }
}

static void update_mesh_normals_flat(float* normals, const float* vertices) {
    for (int i = 0; i < TERRAIN_TRIANGLE_COUNT; i++) {
        int tri_vertex = i * 9;
        Vector3 p0 = {vertices[tri_vertex + 0], vertices[tri_vertex + 1], vertices[tri_vertex + 2]};
        Vector3 p1 = {vertices[tri_vertex + 3], vertices[tri_vertex + 4], vertices[tri_vertex + 5]};
        Vector3 p2 = {vertices[tri_vertex + 6], vertices[tri_vertex + 7], vertices[tri_vertex + 8]};
        Vector3 e1 = Vector3Subtract(p1, p0);
        Vector3 e2 = Vector3Subtract(p2, p0);
        Vector3 normal = Vector3Normalize(Vector3CrossProduct(e1, e2));

        for (int j = 0; j < 3; j++) {
            int normal_vertex = tri_vertex + j * 3;
            normals[normal_vertex + 0] = normal.x;
            normals[normal_vertex + 1] = normal.y;
            normals[normal_vertex + 2] = normal.z;
        }
    }
}

static void expand_mesh_normals_flat(float* dst_normals, const float* src_normals, const unsigned short* indices) {
    for (int i = 0; i < FLAT_VERTEX_COUNT; i++) {
        int src_index = indices[i];
        int src_normal = src_index * 3;
        int dst_normal = i * 3;
        dst_normals[dst_normal + 0] = src_normals[src_normal + 0];
        dst_normals[dst_normal + 1] = src_normals[src_normal + 1];
        dst_normals[dst_normal + 2] = src_normals[src_normal + 2];
    }
}

static void
build_mesh_smooth(Mesh* dst_mesh, const float* vertices, const float* src_normals, const Color* src_colors, const float* src_texcoords, int vertex_count) {
    MEMCPY4(dst_mesh->vertices, vertices, sizeof(float) * vertex_count * 3);
    MEMCPY4(dst_mesh->normals, src_normals, sizeof(float) * vertex_count * 3);
    MEMCPY4(dst_mesh->texcoords, src_texcoords, sizeof(float) * vertex_count * 2);
    MEMCPY(dst_mesh->colors, src_colors, sizeof(Color) * vertex_count);
}

static void build_mesh_flat(Mesh* dst_mesh, const float* flat_vertices, const float* src_normals, const Color* src_colors) {
    MEMCPY4(dst_mesh->vertices, flat_vertices, sizeof(float) * FLAT_VERTEX_COUNT * 3);
    MEMCPY4(dst_mesh->normals, src_normals, sizeof(float) * FLAT_VERTEX_COUNT * 3);
    MEMCPY(dst_mesh->colors, src_colors, sizeof(Color) * FLAT_VERTEX_COUNT);
}

#define LANE_MASK_TEXTURE_WIDTH 8
#define LANE_MASK_TEXTURE_HEIGHT 8
#define LANE_MASK_STRIPE_OFFSET 4
#define LANE_MASK_WIDTH_TEXELS 1
#define LANE_MASK_TEXCOORD_S_CENTER (0.5f / (float)LANE_MASK_TEXTURE_WIDTH)
#define LANE_MASK_TEXCOORD_FRONT_LANE_INNER_EDGE ((float)LANE_MASK_STRIPE_OFFSET / (float)LANE_MASK_TEXTURE_HEIGHT)
#define LANE_MASK_TEXCOORD_MID_LANE_CENTER (((float)LANE_MASK_STRIPE_OFFSET + 0.5f) / (float)LANE_MASK_TEXTURE_HEIGHT)
#define LANE_MASK_TEXCOORD_BACK_LANE_INNER_EDGE (((float)LANE_MASK_STRIPE_OFFSET + (float)LANE_MASK_WIDTH_TEXELS) / (float)LANE_MASK_TEXTURE_HEIGHT)

#define RGBA4444_WHITE_MASK 0xFFF0
#define RGBA4444_ALPHA_MASK 0x000F
#define ALPHA_NONE 0x0000
#define ALPHA_SOFT 0x0003
#define ALPHA_MEDIUM 0x0007
#define ALPHA_FULL 0x000F

//TODO: thought this would be fun, but maybe its silly... makes the procedural texture a little more explicit at least
static const unsigned short GLOW_LUT[LANE_MASK_TEXTURE_HEIGHT] = {
    // FOR 8x8 LANE MASK TEXTURE ONLY! BIGGER TEXTURES -> FINER GLOWS (I THINK)
    ALPHA_NONE,
    ALPHA_NONE,
    ALPHA_SOFT,
    ALPHA_MEDIUM,
    ALPHA_FULL,
    ALPHA_MEDIUM,
    ALPHA_SOFT,
    ALPHA_NONE,
};

static void fill_lane_mask_texcoords(float* texcoords, int point_count) {
    float s = LANE_MASK_TEXCOORD_S_CENTER;
    for (int i = 0; i < LANE_COUNT; i++) {
        float t = (float)i + LANE_MASK_TEXCOORD_MID_LANE_CENTER;
        if (i == 0) {
            t = LANE_MASK_TEXCOORD_FRONT_LANE_INNER_EDGE;
        } else if (i == LANE_COUNT - 1) {
            t = (float)i + LANE_MASK_TEXCOORD_BACK_LANE_INNER_EDGE;
        }
        for (int j = 0; j < point_count; j++) {
            int k = i * point_count + j;
            texcoords[k * 2 + 0] = s;
            texcoords[k * 2 + 1] = t;
        }
    }
}

static Texture2D build_lane_mask(float* texcoords, int point_count) {
    Image image = {
        .data = RL_CALLOC(LANE_MASK_TEXTURE_WIDTH * LANE_MASK_TEXTURE_HEIGHT, sizeof(unsigned short)),
        .width = LANE_MASK_TEXTURE_WIDTH,
        .height = LANE_MASK_TEXTURE_HEIGHT,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R4G4B4A4,
    };
    unsigned short* pixels = (unsigned short*)image.data;
    fill_lane_mask_texcoords(texcoords, point_count);
    for (int i = 0; i < LANE_MASK_TEXTURE_HEIGHT; i++) {
        unsigned short pixel = RGBA4444_WHITE_MASK | ALPHA_NONE;
        if (i >= LANE_MASK_STRIPE_OFFSET && i < LANE_MASK_STRIPE_OFFSET + LANE_MASK_WIDTH_TEXELS) {
            pixel = RGBA4444_WHITE_MASK | ALPHA_FULL;
        }
        for (int j = 0; j < LANE_MASK_TEXTURE_WIDTH; j++) {
            pixels[i * LANE_MASK_TEXTURE_WIDTH + j] = pixel;
        }
    }
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
    return texture;
}

static Texture2D build_lane_mask_glow(float* texcoords, int point_count) {
    Image image = {
        .data = RL_CALLOC(LANE_MASK_TEXTURE_WIDTH * LANE_MASK_TEXTURE_HEIGHT, sizeof(unsigned short)),
        .width = LANE_MASK_TEXTURE_WIDTH,
        .height = LANE_MASK_TEXTURE_HEIGHT,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R4G4B4A4,
    };
    unsigned short* pixels = (unsigned short*)image.data;
    fill_lane_mask_texcoords(texcoords, point_count);
    for (int i = 0; i < LANE_MASK_TEXTURE_HEIGHT; i++) {
        unsigned short pixel = RGBA4444_WHITE_MASK | (GLOW_LUT[i] & RGBA4444_ALPHA_MASK);
        for (int j = 0; j < LANE_MASK_TEXTURE_WIDTH; j++) {
            pixels[i * LANE_MASK_TEXTURE_WIDTH + j] = pixel;
        }
    }
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
    return texture;
}

#define ONSET_STRENGTH_MIN 0.0f
#define ONSET_STRENGTH_MAX 0.025f
#define ONSET_ATTACK_RATE 0.95f
#define ONSET_DECAY_RATE 0.10f
#define ONSET_LAG_FRAMES 1
#define ONSET_ENVELOPE_RADIUS 2
#define LIGHT0_DIFFUSE_MIN 0.0f
#define LIGHT0_DIFFUSE_MAX 1.5f
#define LIGHT0_MARKER_RADIUS 0.5f
#define LIGHT0_MARKER_RING_COUNT 3
#define LIGHT0_MARKER_SEGMENTS 16

static float onset_interpolation_factor = 0.0f;
static float onset_interpolation_factor_history[ANALYSIS_FFT_HISTORY_FRAME_COUNT] = {0};
static float light0_diffuse[4] = {LIGHT0_DIFFUSE_MIN, LIGHT0_DIFFUSE_MIN, LIGHT0_DIFFUSE_MIN, 1.0f};
static Vector3 light0_position = {1.330f, 1.345f, -1.418f};

static void update_onset_interpolation_factor_fft(FFTData* fft_data) {
    // librosa onset_strength convention:
    // https://librosa.org/doc/main/generated/librosa.onset.onset_strength.html
    // TODO: onset_detect would be a later peak-pick stage is more complicated...
    // https://librosa.org/doc/main/generated/librosa.onset.onset_detect.html
    int current_index = (fft_data->history_pos - 1 + ANALYSIS_FFT_HISTORY_FRAME_COUNT) % ANALYSIS_FFT_HISTORY_FRAME_COUNT;
    if (fft_data->frame_index <= ONSET_LAG_FRAMES) {
        onset_interpolation_factor_history[current_index] = onset_interpolation_factor;
        return;
    }

    int lag_index = (fft_data->history_pos - 1 - ONSET_LAG_FRAMES + ANALYSIS_FFT_HISTORY_FRAME_COUNT) % ANALYSIS_FFT_HISTORY_FRAME_COUNT;
    float* spectrum = fft_data->spectrum_history_levels[current_index];
    float* lag_spectrum = fft_data->spectrum_history_levels[lag_index];
    float flux_sum = 0.0f;
    for (int i = 0; i < ANALYSIS_SPECTRUM_BIN_COUNT; i++) {
        float ref = lag_spectrum[i];
        for (int j = -ONSET_ENVELOPE_RADIUS; j <= ONSET_ENVELOPE_RADIUS; j++) {
            int k = i + j;
            if (k < 0 || k >= ANALYSIS_SPECTRUM_BIN_COUNT) {
                continue;
            }
            ref = FMAXF(ref, lag_spectrum[k]);
        }
        float flux = spectrum[i] - ref;
        if (flux > 0.0f) {
            flux_sum += flux;
        }
    }
    float onset_strength = flux_sum / (float)ANALYSIS_SPECTRUM_BIN_COUNT;
    float onset_strength_normalized = CLAMP((onset_strength - ONSET_STRENGTH_MIN) / (ONSET_STRENGTH_MAX - ONSET_STRENGTH_MIN), 0.0f, 1.0f);
    float onset_rate = ONSET_DECAY_RATE;
    if (onset_strength_normalized > onset_interpolation_factor) {
        onset_rate = ONSET_ATTACK_RATE;
    }
    onset_interpolation_factor = LERP(onset_interpolation_factor, onset_strength_normalized, onset_rate);
    onset_interpolation_factor = CLAMP(onset_interpolation_factor, 0.0f, 1.0f);
    onset_interpolation_factor_history[current_index] = onset_interpolation_factor;
}

static void update_diffuse_strength(void) {
    float light0_diffuse_strength = LERP(LIGHT0_DIFFUSE_MIN, LIGHT0_DIFFUSE_MAX, onset_interpolation_factor);
    light0_diffuse[0] = light0_diffuse_strength;
    light0_diffuse[1] = light0_diffuse_strength;
    light0_diffuse[2] = light0_diffuse_strength;
    light0_diffuse[3] = 1.0f;
}

static inline void draw_light_position_marker(void) {
    for (int i = 0; i < LIGHT0_MARKER_SEGMENTS; i++) {
        float angle_0_rad = ((float)i / (float)LIGHT0_MARKER_SEGMENTS) * (2.0f * PI);
        float angle_1_rad = ((float)(i + 1) / (float)LIGHT0_MARKER_SEGMENTS) * (2.0f * PI);
        float ring_cos_0 = COSF(angle_0_rad) * LIGHT0_MARKER_RADIUS;
        float ring_sin_0 = SINF(angle_0_rad) * LIGHT0_MARKER_RADIUS;
        float ring_cos_1 = COSF(angle_1_rad) * LIGHT0_MARKER_RADIUS;
        float ring_sin_1 = SINF(angle_1_rad) * LIGHT0_MARKER_RADIUS;
        for (int j = 0; j < LIGHT0_MARKER_RING_COUNT; j++) {
            float ring_angle_rad = ((float)j / (float)LIGHT0_MARKER_RING_COUNT) * PI;
            float ring_x = COSF(ring_angle_rad);
            float ring_y = SINF(ring_angle_rad);
            Vector3 point_0 = {
                light0_position.x + ring_cos_0 * ring_x,
                light0_position.y + ring_cos_0 * ring_y,
                light0_position.z + ring_sin_0,
            };
            Vector3 point_1 = {
                light0_position.x + ring_cos_1 * ring_x,
                light0_position.y + ring_cos_1 * ring_y,
                light0_position.z + ring_sin_1,
            };
            rlDisableDepthTest();
            DrawLine3D(point_0, point_1, NEON_CARROT);
            rlEnableDepthTest();
        }
    }
}

#define CAMERA_FOVY_MIN 0.1f
#define CAMERA_FOVY_MAX 10.0f
#define CAMERA_FOVY_VELOCITY 6.0f
#define CAMERA_ORBIT_VELOCITY 2.0f
#define CAMERA_PITCH_MIN (-PI / 2.0f + 0.1f)
#define CAMERA_PITCH_MAX (PI / 2.0f - 0.1f)
#define BUTTON_DOWN_NAV_INITIAL_DELAY_SECONDS 0.35f
//TODO: hardmode-> find the exact lane advance cadence for this value, lock it to playback rate a default,
// then somehow allow for playback snippets to get fed to the device at player controlled update rates...??
#define BUTTON_DOWN_NAV_INTERVAL_SECONDS (0.125f / 2.0f)

static inline void update_camera_orbit(Camera3D* camera, float dt) {
    Vector3 dist_from_target = Vector3Subtract(camera->position, camera->target);
    float orbit_radius = Vector3Length(dist_from_target);
    float yaw = atan2f(dist_from_target.z, dist_from_target.x);
    float ground_radius = sqrtf(dist_from_target.x * dist_from_target.x + dist_from_target.z * dist_from_target.z);
    float pitch = atan2f(dist_from_target.y, ground_radius);
    float fovy = camera->fovy;

    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT))
        yaw += CAMERA_ORBIT_VELOCITY * dt;
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT))
        yaw -= CAMERA_ORBIT_VELOCITY * dt;
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP))
        pitch += CAMERA_ORBIT_VELOCITY * dt;
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN))
        pitch -= CAMERA_ORBIT_VELOCITY * dt;
    if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) > 0.0f)
        fovy -= GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) * CAMERA_FOVY_VELOCITY * dt;
    if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_TRIGGER) > 0.0f)
        fovy += GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_TRIGGER) * CAMERA_FOVY_VELOCITY * dt;

    pitch = Clamp(pitch, CAMERA_PITCH_MIN, CAMERA_PITCH_MAX);
    fovy = Clamp(fovy, CAMERA_FOVY_MIN, CAMERA_FOVY_MAX);
    camera->position.x = camera->target.x + orbit_radius * cosf(pitch) * cosf(yaw);
    camera->position.y = camera->target.y + orbit_radius * sinf(pitch);
    camera->position.z = camera->target.z + orbit_radius * cosf(pitch) * sinf(yaw);
    camera->fovy = fovy;
}

#define PADMOUSE_DEADZONE 0.20f
#define PADMOUSE_SPEED 400.0f
#define PADMOUSE_HOVER_RADIUS_PIXELS 20.0f
static Vector2 padmouse_pos = {0.5f * SCREEN_WIDTH, 0.5f * SCREEN_HEIGHT};

static inline void update_padmouse(float dt, const Camera3D* camera) {
    static bool light0_grabbed = false;
    float axis_x = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
    float axis_y = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
    if (FABSF(axis_x) < PADMOUSE_DEADZONE)
        axis_x = 0.0f;
    if (FABSF(axis_y) < PADMOUSE_DEADZONE)
        axis_y = 0.0f;

    padmouse_pos.x += axis_x * PADMOUSE_SPEED * dt;
    padmouse_pos.y += axis_y * PADMOUSE_SPEED * dt;
    padmouse_pos.x = CLAMP(padmouse_pos.x, 0.0f, (float)(GetScreenWidth() - 1));
    padmouse_pos.y = CLAMP(padmouse_pos.y, 0.0f, (float)(GetScreenHeight() - 1));
    Vector2 light_screen_pos = GetWorldToScreen(light0_position, *camera);
    float light_dx = padmouse_pos.x - light_screen_pos.x;
    float light_dy = padmouse_pos.y - light_screen_pos.y;
    float light_dist_sq = light_dx * light_dx + light_dy * light_dy;
    float light_hover_radius_sq = PADMOUSE_HOVER_RADIUS_PIXELS * PADMOUSE_HOVER_RADIUS_PIXELS;
    if (light0_grabbed && !IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
        light0_grabbed = false;
    } else if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) && light_dist_sq <= light_hover_radius_sq) {
        light0_grabbed = true;
    }

    if (light0_grabbed) {
        Ray screen_ray = GetScreenToWorldRay(padmouse_pos, *camera);
        Vector3 plane_normal = Vector3Normalize(Vector3Subtract(camera->position, camera->target));
        float plane_dist = Vector3DotProduct(light0_position, plane_normal);
        float denom = Vector3DotProduct(screen_ray.direction, plane_normal);
        float intersection_dist = (plane_dist - Vector3DotProduct(screen_ray.position, plane_normal)) / denom;
        if (intersection_dist > 0.0f) {
            light0_position = Vector3Add(screen_ray.position, Vector3Scale(screen_ray.direction, intersection_dist));
        }
    }
}

static inline void draw_padmouse(void) {
    DrawCircleLines((int)padmouse_pos.x, (int)padmouse_pos.y, 7.0f, Fade(WHITE, 0.8f));
    DrawPixel((int)padmouse_pos.x, (int)padmouse_pos.y, WHITE);
}

static float sticky_nav_at[2] = {0};

static inline void reset_sticky_nav(void) {
    sticky_nav_at[0] = 0.0f;
    sticky_nav_at[1] = 0.0f;
}

static inline int sticky_nav(int button) {
    int i = (button == GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    float now = (float)GetTime();
    int nav_flag = 0;
    if (IsGamepadButtonPressed(0, button)) {
        sticky_nav_at[i] = now + BUTTON_DOWN_NAV_INITIAL_DELAY_SECONDS;
        nav_flag = 1;
    } else if (!IsGamepadButtonDown(0, button)) {
        sticky_nav_at[i] = 0.0f;
    } else if (now >= sticky_nav_at[i]) {
        sticky_nav_at[i] = now + BUTTON_DOWN_NAV_INTERVAL_SECONDS;
        nav_flag = 1;
    }
    return nav_flag;
}

static void rebuild_envelope_history_from_wave(void) {
    for (int i = 0; i < ANALYSIS_WINDOW_SIZE_IN_FRAMES; i++) {
        int src = (wave_cursor + i) % wave.frameCount;
        analysis_window_samples[i] = (float)wave_pcm16[src] / ANALYSIS_PCM16_UPPER_BOUND;
    }

    for (int i = 0; i < LANE_COUNT; i++) {
        int start_frame = (wave_cursor + wave.frameCount - ((i * AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES) % wave.frameCount)) % wave.frameCount;

        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            int window_sample = (j * (ANALYSIS_WINDOW_SIZE_IN_FRAMES - 1)) / (LANE_POINT_COUNT - 1);
            int src = (start_frame + window_sample) % wave.frameCount;
            lane_point_values[i][j] = (float)wave_pcm16[src] / ANALYSIS_PCM16_UPPER_BOUND;
        }
    }
}

static Font font = {0};
#define WAVE_CURSOR_BLINK_RATE 8.0f
static Model* wave_cursor_model = NULL;
static Texture2D wave_cursor_texture = {0};
static Color wave_cursor_colors[MESH_VERTEX_COUNT] = {0};
#define FORWARD 1
#define BACKWARD -1

static void draw_paused_wave_cursor_lane_marker(void) {
    float blink = SINF((float)GetTime() * WAVE_CURSOR_BLINK_RATE);
    int lane_index = seek_delta_chunks;
    float z_shift = 0.0f;
    Color marker_color = CLITERAL(Color){
        (unsigned char)((float)NEON_CARROT.r * (0.725f + 0.275f * blink)),
        (unsigned char)((float)NEON_CARROT.g * (0.725f + 0.275f * blink)),
        (unsigned char)((float)NEON_CARROT.b * (0.725f + 0.275f * blink)),
        (unsigned char)(175.5f + 79.5f * blink),
    };
    if (lane_index < 0) {
        lane_index = 0;
        z_shift = -0.75f * LANE_SPACING_SCALE;
    } else if (lane_index >= LANE_COUNT) {
        lane_index = LANE_COUNT - 1;
        z_shift = 0.75f * LANE_SPACING_SCALE;
    }
    for (int i = 0; i < MESH_VERTEX_COUNT; i++) {
        wave_cursor_colors[i] = BLANK;
    }
    for (int i = 0; i < LANE_POINT_COUNT; i++) {
        wave_cursor_colors[lane_index * LANE_POINT_COUNT + i] = marker_color;
    }

    wave_cursor_model->meshes[0].colors = (unsigned char*)wave_cursor_colors;
    wave_cursor_model->materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = wave_cursor_texture.id;
    rlDisableDepthTest();
    rlDisableBackfaceCulling();
    DrawModelEx(*wave_cursor_model, (Vector3){0.0f, 0.0f, z_shift}, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);
    rlEnableBackfaceCulling();
    rlEnableDepthTest();
    wave_cursor_model->materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = 0;
}

static inline void draw_wave_cursor_wheel_hud_row(const char* s, float x, float y, Color c) {
    char g[2] = {0};
    for (int i = 0; s[i]; i++) {
        if (s[i] == ' ')
            continue;
        g[0] = s[i];
        DrawTextEx(font, g, (Vector2){x + (float)i * 7.0f, y}, FONT_SIZE, 0.0f, c);
    }
}

static inline void draw_inspection_hud_wheel_row(int indent, float y, int row_offset, Color row_color) {
    int row_delta_chunks = seek_delta_chunks + row_offset;
    int row_delta_ms = (row_delta_chunks * AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES * MILLISECONDS_PER_SECOND) / SRC_SAMPLE_RATE;
    bool selected = row_delta_chunks == 0;
    int display_cursor = (paused_wave_cursor + row_delta_chunks * AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES) % wave.frameCount;
    if (display_cursor < 0)
        display_cursor += wave.frameCount;
    int display_cursor_ms = SAMPLE_CURSOR_TO_MS(display_cursor);
    Color color = selected ? SUNFLOWER : row_color;
    float x = 376.0f + (float)indent * 7.0f;
    if (row_offset == 0)
        draw_wave_cursor_wheel_hud_row(">", x - 14.0f, y, SUNFLOWER);
    draw_wave_cursor_wheel_hud_row(TextFormat("%6d", display_cursor), x, y, color);
    DrawTextEx(font, ":", (Vector2){x + 42.0f, y}, FONT_SIZE, 0.0f, color);
    draw_wave_cursor_wheel_hud_row(TextFormat("%+04d", row_delta_chunks), x + 56.0f, y, color);
    DrawTextEx(font, "c", (Vector2){x + 84.0f, y}, FONT_SIZE, 0.0f, color);
    draw_wave_cursor_wheel_hud_row(TextFormat("%+04d", row_delta_ms), x + 98.0f, y, color);
    DrawTextEx(font, "ms", (Vector2){x + 126.0f, y}, FONT_SIZE, 0.0f, color);
    draw_wave_cursor_wheel_hud_row(TextFormat(MMSSMMM_FMT, MMSSMMM_ARGS(display_cursor_ms)), x + 147.0f, y, color);
}

static void draw_playback_inspection_hud(void) {
    int cursor = is_paused ? paused_wave_cursor : wave_cursor;
    int cursor_ms = SAMPLE_CURSOR_TO_MS(cursor);
    Color top_color = is_paused ? SUNFLOWER : MARINER;
    draw_wave_cursor_wheel_hud_row(TextFormat("[CURSOR:%6d]", cursor), 7.0f + 20.0f, 25.0f, top_color);
    draw_wave_cursor_wheel_hud_row(TextFormat("[STEP:%4d]", AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES), 140.0f + 20.0f, 25.0f, top_color);
    draw_wave_cursor_wheel_hud_row(TextFormat("[SAMPLE_RATE:%5d]", SRC_SAMPLE_RATE), 244.0f + 20.0f, 25.0f, top_color);
    draw_wave_cursor_wheel_hud_row(TextFormat("[TIME:" MMSSMMM_FMT "]", MMSSMMM_ARGS(cursor_ms)), 402.0f + 20.0f, 25.0f, top_color);
    draw_wave_cursor_wheel_hud_row(is_paused ? "[PAUSED]" : "[PLAYING]", 533.0f + 20.0f, 25.0f, is_paused ? NEON_CARROT : SUNFLOWER);
    if (is_paused) {
        draw_wave_cursor_wheel_hud_row("        [...]", 376.0f, 352.0f - 5.0f, seek_delta_chunks < -2 ? SUNFLOWER : MARINER);
        draw_inspection_hud_wheel_row(6, 368.0f - 5.0f, 2, MARINER);
        draw_inspection_hud_wheel_row(4, 384.0f - 5.0f, 1, MARINER);
        draw_inspection_hud_wheel_row(2, 400.0f - 5.0f, 0, MARINER);
        draw_inspection_hud_wheel_row(4, 416.0f - 5.0f, -1, MARINER);
        draw_inspection_hud_wheel_row(6, 432.0f - 5.0f, -2, MARINER);
        draw_wave_cursor_wheel_hud_row("        [...]", 376.0f, 448.0f - 5.0f, seek_delta_chunks > 2 ? SUNFLOWER : MARINER);
    }
}

static void update_playback_controls(void) {
    if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
        reset_sticky_nav();
        if (!is_paused) {
            is_paused = true;
            wave_cursor = (wave_cursor + wave.frameCount - AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES) % wave.frameCount;
            paused_wave_cursor = wave_cursor;
            seek_delta_chunks = 0;
            for (int i = 0; i < MAX_DRAIN_CHUNK_COUNT; i++) {
                while (!IsAudioStreamProcessed(audio_stream)) {
                    /* KEEP DRAINING! */
                }
                UpdateAudioStream(audio_stream, drain_chunk_samples, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
            }
            PauseAudioStream(audio_stream);
            rebuild_envelope_history_from_wave();
        } else {
            is_paused = false;
            PlayAudioStream(audio_stream);
            while (IsAudioStreamProcessed(audio_stream)) {
                for (int i = 0; i < AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES; i++) {
                    resume_chunk_samples[i] = wave_pcm16[wave_cursor];
                    if (++wave_cursor >= wave.frameCount) {
                        wave_cursor = 0;
                    }
                }
                UpdateAudioStream(audio_stream, resume_chunk_samples, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);

                for (int i = 0; i < ANALYSIS_WINDOW_SIZE_IN_FRAMES; i++) {
                    analysis_window_samples[i] = (float)resume_chunk_samples[i] / ANALYSIS_PCM16_UPPER_BOUND;
                }

                advance_lane_history(&lane_point_values[0][0], LANE_POINT_COUNT);
                for (int i = 0; i < LANE_POINT_COUNT; i++) {
                    lane_point_values[0][i] = analysis_window_samples[(i * (ANALYSIS_WINDOW_SIZE_IN_FRAMES - 1)) / (LANE_POINT_COUNT - 1)];
                }
            }
        }
    }
    if (is_paused && sticky_nav(GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
        wave_cursor = (wave_cursor + wave.frameCount - AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES) % wave.frameCount;
        seek_delta_chunks--;
        rebuild_envelope_history_from_wave();
    } else if (is_paused && sticky_nav(GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
        wave_cursor = (wave_cursor + AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES) % wave.frameCount;
        seek_delta_chunks++;
        rebuild_envelope_history_from_wave();
    }
}

#define DEFAULT_AUDIO_TRACK_SHADERTOY_EXPERIMENT 0
#define DEFAULT_AUDIO_TRACK_ELECTRONEBULAE 1
#define DEFAULT_AUDIO_TRACK_DDS_FFM 2
#define DEFAULT_AUDIO_TRACK_RAMA 3
#define DEFAULT_AUDIO_TRACK_CT_LOR 4
#define DEFAULT_AUDIO_TRACK_AT_UNTITLED 5
#define DEFAULT_AUDIO_TRACK_TJ_SAYO 6
#define AUDIO_TRACK_COUNT 7

#define AUDIO_TRACK_PATH(track_index)                                                                                                                          \
    ((track_index) == DEFAULT_AUDIO_TRACK_SHADERTOY_EXPERIMENT ? RD_SHADERTOY_EXPERIMENT_22K_WAV                                                               \
     : (track_index) == DEFAULT_AUDIO_TRACK_ELECTRONEBULAE     ? RD_SHADERTOY_ELECTRONEBULAE_ONE_FOURTH_22K_WAV                                                \
     : (track_index) == DEFAULT_AUDIO_TRACK_DDS_FFM            ? RD_DDS_FFM_22K_WAV                                                                            \
     : (track_index) == DEFAULT_AUDIO_TRACK_RAMA               ? RD_RAMA_22K_WAV                                                                               \
     : (track_index) == DEFAULT_AUDIO_TRACK_CT_LOR             ? RD_CT_LOR_22K_WAV                                                                             \
     : (track_index) == DEFAULT_AUDIO_TRACK_AT_UNTITLED        ? RD_AT_UNTITLED_22K_WAV                                                                        \
                                                               : RD_TJ_SAYO_22K_WAV)

#define LOAD_AUDIO_TRACK(track_index)                                                                                                                          \
    do {                                                                                                                                                       \
        audio_track_index = (track_index);                                                                                                                     \
        wave = LoadWave(AUDIO_TRACK_PATH(audio_track_index));                                                                                                  \
    } while (0)

static int audio_track_index = DEFAULT_AUDIO_TRACK_SHADERTOY_EXPERIMENT;

static inline void update_audio_track_cycle(void) {
    int dir = 0;
    if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT)) {
        dir = BACKWARD;
    } else if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP)) {
        dir = FORWARD;
    }
    if (dir == 0) {
        return;
    }

    audio_track_index += dir;
    if (audio_track_index < 0) {
        audio_track_index = AUDIO_TRACK_COUNT - 1;
    } else if (audio_track_index >= AUDIO_TRACK_COUNT) {
        audio_track_index = 0;
    }

    StopAudioStream(audio_stream);
    UnloadWave(wave);
    wave = LoadWave(AUDIO_TRACK_PATH(audio_track_index));
    WaveFormat(&wave, SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    wave_pcm16 = (int16_t*)wave.data;
    wave_cursor = 0;
    paused_wave_cursor = 0;
    seek_delta_chunks = 0;
    is_paused = false;
    PlayAudioStream(audio_stream);
}

#define PITCH_CLASS_COUNT 12
#define PITCH_CLASS_INVERSE_LN_2 1.4426950408889634f
#define PITCH_CLASS_SEMITONES_PER_OCTAVE 12.0f
#define PITCH_CLASS_TUNING_RADIUS_SEMITONES 0.85f
#define PITCH_CLASS_VISIBILITY_MIN 0.02f
#define PITCH_CLASS_VISIBILITY_MAX 0.75f
#define C9 8372.02f
#define Db9 8870.0f
#define D9 9398.0f
#define Eb9 9974.0f
#define E9 10548.0f
#define F9 11176.0f
#define Gb9 11840.0f
#define G9 12544.0f
#define Ab9 13290.0f
#define A9 14080.0f
#define Bb9 14918.0f
#define B9 15804.0f
#define REF_HZ_LUT {C9, Db9, D9, Eb9, E9, F9, Gb9, G9, Ab9, A9, Bb9, B9}
#define REF_HZ_LOOKUP(index) (((const float[PITCH_CLASS_COUNT])REF_HZ_LUT)[(index)])
#define ACCIDENTALS_LUT {1.0f, 0.5f, 1.0f, 0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.5f, 1.0f, 0.5f, 1.0f}
#define ACCIDENTAL_LOOKUP(index) (((const float[PITCH_CLASS_COUNT])ACCIDENTALS_LUT)[(index)])
#define WUXING_COLOR_LOOKUP(index) (((const Color[PITCH_CLASS_COUNT])WUXING_PITCH_CLASS_LUT)[(index)])

static Color sample_pitch_class_palette(unsigned char chroma_index, float confidence) {
    Color base_color = WUXING_COLOR_LOOKUP(chroma_index);
    float visibility = CLAMP((confidence - PITCH_CLASS_VISIBILITY_MIN) / (PITCH_CLASS_VISIBILITY_MAX - PITCH_CLASS_VISIBILITY_MIN), 0.0f, 1.0f);
    if (visibility <= 0.0f) {
        return BLACK;
    }

    visibility = POWF(visibility, 0.45f);
    float accidental_gain = 0.70f + 0.30f * ACCIDENTAL_LOOKUP(chroma_index);
    float brightness = (0.35f + 0.65f * visibility) * accidental_gain;
    brightness = CLAMP(brightness, 0.0f, 1.0f);

    return (Color){
        (unsigned char)((float)base_color.r * brightness),
        (unsigned char)((float)base_color.g * brightness),
        (unsigned char)((float)base_color.b * brightness),
        DRAW_COLOR_CHANNEL_MAX,
    };
}

static void update_mesh_colors_pitch_class(Color* colors, const unsigned char* chroma_index_field, const float* chroma_strength_field, int point_count) {
    for (int i = 0; i < LANE_COUNT; i++) {
        for (int j = 0; j < point_count; j++) {
            int k = i * point_count + j;
            colors[k] = sample_pitch_class_palette(chroma_index_field[k], chroma_strength_field[k]);
        }
    }
}

static inline Vector2 spectrum_band_point_sample_bin_bounds(int point_index, int point_count, int band_bin_min, int band_bin_max) {
    int point_span = point_count - 1;
    int bin_span = band_bin_max - band_bin_min;
    int center_bin = band_bin_min;
    int left_neighbor_center_bin = band_bin_min;
    int right_neighbor_center_bin = band_bin_min;

    if (point_count > 1) {
        center_bin = band_bin_min + (point_index * bin_span) / point_span;
        left_neighbor_center_bin = (point_index > 0) ? band_bin_min + ((point_index - 1) * bin_span) / point_span : center_bin;
        right_neighbor_center_bin = (point_index < point_span) ? band_bin_min + ((point_index + 1) * bin_span) / point_span : center_bin;
    }

    int sample_bin_min = (point_index > 0) ? ((left_neighbor_center_bin + center_bin) / 2) : band_bin_min;
    int sample_bin_max = (point_index < point_span) ? ((center_bin + right_neighbor_center_bin + 1) / 2) : band_bin_max;
    sample_bin_min = CLAMP(sample_bin_min, band_bin_min, band_bin_max);
    sample_bin_max = CLAMP(sample_bin_max, sample_bin_min, band_bin_max);
    return (Vector2){(float)sample_bin_min, (float)sample_bin_max};
}

static void build_pitch_class_color_field(unsigned char* chroma_index_field,
                                          float* chroma_strength_field,
                                          const float* front_lane_point_values,
                                          const float* bin_levels,
                                          int point_count,
                                          int band_bin_min,
                                          int band_bin_max) {
    // https://librosa.org/doc/main/generated/librosa.feature.chroma_stft.html
    for (int i = 0; i < point_count; i++) {
        int point_span = point_count - 1;
        int bin_span = band_bin_max - band_bin_min;
        int center_bin = band_bin_min;
        Vector2 point_bin_bounds = spectrum_band_point_sample_bin_bounds(i, point_count, band_bin_min, band_bin_max);
        int bin_min = point_bin_bounds.x;
        int bin_max = point_bin_bounds.y;
        if (point_count > 1) {
            center_bin = band_bin_min + (i * bin_span) / point_span;
        }

        float chroma_energy[PITCH_CLASS_COUNT] = {0};
        float band_energy_sum = 0.0f;
        float band_peak = 0.0f;
        float locality_radius_bins = FMAXF(0.5f * (float)(bin_max - bin_min + 1), 1.0f);

        for (int j = bin_min; j <= bin_max; j++) {
            float bin_hz = ((float)j * (float)ANALYSIS_SAMPLE_RATE) / (float)ANALYSIS_WINDOW_SIZE_IN_FRAMES;
            float bin_level = bin_levels[j];
            if (bin_level <= 0.0f) {
                continue;
            }

            float locality_gain = 1.0f - (FABSF((float)j - (float)center_bin) / locality_radius_bins);
            locality_gain = CLAMP(locality_gain, 0.0f, 1.0f);
            locality_gain = LERP(0.35f, 1.0f, locality_gain);

            band_energy_sum += bin_level;
            band_peak = FMAXF(band_peak, bin_level);

            for (int k = 0; k < PITCH_CLASS_COUNT; k++) {
                float ref_hz = REF_HZ_LOOKUP(k);
                float octave_offset = FLOORF((LOGF(ref_hz / bin_hz) * PITCH_CLASS_INVERSE_LN_2) + 0.5f);
                float folded_hz = ref_hz / POWF(2.0f, octave_offset);
                float semitone_error = FABSF(PITCH_CLASS_SEMITONES_PER_OCTAVE * LOGF(bin_hz / folded_hz) * PITCH_CLASS_INVERSE_LN_2);
                float tuning_gain = CLAMP(1.0f - (semitone_error / PITCH_CLASS_TUNING_RADIUS_SEMITONES), 0.0f, 1.0f);
                tuning_gain = tuning_gain * tuning_gain * (3.0f - 2.0f * tuning_gain);
                if (tuning_gain <= 0.0f) {
                    continue;
                }

                chroma_energy[k] += bin_level * locality_gain * tuning_gain;
            }
        }

        int best_chroma = 0;
        float best_energy = 0.0f;
        float next_energy = 0.0f;
        for (int j = 0; j < PITCH_CLASS_COUNT; j++) {
            float energy = chroma_energy[j];
            if (energy > best_energy) {
                next_energy = best_energy;
                best_energy = energy;
                best_chroma = j;
            } else if (energy > next_energy) {
                next_energy = energy;
            }
        }

        float ridge_level = front_lane_point_values[i];
        float dominance = 0.0f;
        float separation = 0.0f;
        if (band_energy_sum > 0.0f) {
            dominance = best_energy / band_energy_sum;
        }
        if (best_energy > 0.0f) {
            separation = (best_energy - next_energy) / best_energy;
        }
        float peak_strength = CLAMP(band_peak * 18.0f, 0.0f, 1.0f);
        float chroma_strength = ridge_level * 0.55f + peak_strength * 0.20f + CLAMP(dominance, 0.0f, 1.0f) * 0.15f + CLAMP(separation, 0.0f, 1.0f) * 0.10f;
        chroma_strength = CLAMP(chroma_strength, 0.0f, 1.0f);
        chroma_strength = POWF(chroma_strength, 0.65f);
        if (best_energy <= 0.0f || ridge_level <= 0.0f) {
            chroma_strength = 0.0f;
        }

        chroma_index_field[i] = (unsigned char)best_chroma;
        chroma_strength_field[i] = chroma_strength;
    }
}

#define WHITE_NOISE_SEED 0x19971110
#define WHITE_NOISE_ALPHA_THRESHOLD 128

// TODO: interesting noise implementatino from late 1990s:
// https://web.archive.org/web/20071223173210/http://www.concentric.net/~Ttwang/tech/inthash.htm
static inline uint32_t jenkins_1997_hash32(uint32_t a) {
    a = (a + 0x7ED55D16) + (a << 12);
    a = (a ^ 0xC761C23C) ^ (a >> 19);
    a = (a + 0x165667B1) + (a << 5);
    a = (a + 0xD3A2646C) ^ (a << 9);
    a = (a + 0xFD7046C5) + (a << 3);
    a = (a ^ 0xB55A4F09) ^ (a >> 16);
    return a;
}

static inline uint8_t jenkins_1997_white_noise_u8(uint32_t x, uint32_t y, uint32_t width, uint32_t seed) {
    uint32_t i = y * width + x;
    return (uint8_t)(jenkins_1997_hash32(i ^ seed) >> 24);
}

static inline uint16_t jenkins_1997_white_noise_rgb565(uint32_t x, uint32_t y, uint32_t width, uint32_t seed) {
    uint8_t v = jenkins_1997_white_noise_u8(x, y, width, seed);
    return (uint16_t)(((v >> 3) << 11) | ((v >> 2) << 5) | (v >> 3));
}

//TODO: bake offline ofc, and would be good chance to study pvr and vq stuff too again, just to be thorough/more practice
static Texture2D build_white_noise_texture(int w, int h) {
    Image image = {
        .data = RL_CALLOC(w * h, sizeof(unsigned short)),
        .width = w,
        .height = h,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R5G6B5,
    };
    unsigned short* pixels = (unsigned short*)image.data;
    for (uint32_t y = 0; y < h; y++) {
        for (uint32_t x = 0; x < w; x++) {
            pixels[y * w + x] = jenkins_1997_white_noise_rgb565(x, y, w, WHITE_NOISE_SEED);
        }
    }
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
    return texture;
}

static inline uint16_t jenkins_1997_white_noise_rgba4444(uint32_t x, uint32_t y, uint32_t width, uint32_t seed) {
    uint8_t v = jenkins_1997_white_noise_u8(x, y, width, seed);
    uint16_t alpha = (v >= WHITE_NOISE_ALPHA_THRESHOLD) ? ALPHA_FULL : ALPHA_NONE;
    return (uint16_t)(RGBA4444_WHITE_MASK | (alpha & RGBA4444_ALPHA_MASK));
}

static const int WHITE_NOISE_SIZE_LUT[8] = {8, 16, 32, 64, 128, 256, 512, 1024};

#define WHITE_NOISE_MASK_DIMS(point_count, texels_per_quad)                                                                                                    \
    ((int[2]){                                                                                                                                                 \
        next_power_of_two_lut(((point_count) - 1) * (texels_per_quad)),                                                                                        \
        next_power_of_two_lut((LANE_COUNT - 1) * (texels_per_quad)),                                                                                           \
    })

static int next_power_of_two_lut(int value) {
    int clamped_value = (value > 1) ? value : 1;
    for (int i = 0; i < 8; i++) {
        if (WHITE_NOISE_SIZE_LUT[i] >= clamped_value) {
            return WHITE_NOISE_SIZE_LUT[i];
        }
    }
    return WHITE_NOISE_SIZE_LUT[7];
}

static void fill_white_noise_mask_texcoords(int w, int h, float* texcoords, int point_count, int texels_per_quad) {
    for (int i = 0; i < LANE_COUNT; i++) {
        for (int j = 0; j < point_count; j++) {
            int k = (i * point_count + j) * 2;
            texcoords[k + 0] = (float)(j * texels_per_quad) / (float)w;
            texcoords[k + 1] = (float)(i * texels_per_quad) / (float)h;
        }
    }
}

static Texture2D build_white_noise_mask(int w, int h, float* texcoords, int point_count, int texels_per_quad) {
    Image image = {
        .data = RL_CALLOC(w * h, sizeof(unsigned short)),
        .width = w,
        .height = h,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R4G4B4A4,
    };
    unsigned short* pixels = (unsigned short*)image.data;
    fill_white_noise_mask_texcoords(w, h, texcoords, point_count, texels_per_quad);
    for (uint32_t y = 0; y < (uint32_t)h; y++) {
        for (uint32_t x = 0; x < (uint32_t)w; x++) {
            pixels[y * w + x] = jenkins_1997_white_noise_rgba4444(x, y, w, WHITE_NOISE_SEED);
        }
    }
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
    return texture;
}

#endif // FFFFTT_H
