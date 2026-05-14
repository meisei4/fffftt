#ifndef FFFFTT_H
#define FFFFTT_H

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "../sh4zam/include/sh4zam/shz_sh4zam.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <dc/perfctr.h>
#include <kos/fs.h>
#include <fcntl.h>

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
#define FMINF(x, y) shz_fminf((x), (y))
#define FABSF(x) shz_fabsf((x))
#define MEMSET(dst, value, size) memset((dst), (value), (size))
#define MEMCPY(dst, src, size) memcpy((dst), (src), (size)) //TODO: Colors... hmmm
#define MEMCPY4(dst, src, size) shz_memcpy4((dst), (src), (size))
#define CLAMP(x, min, max) shz_clampf((x), (min), (max))
#define LERP(a, b, t) shz_lerpf((a), (b), (t))
#define MAXI(x, y) ((x) > (y) ? (x) : (y))
#define MINI(x, y) ((x) < (y) ? (x) : (y))
#define WRAP(pos, size)                                                                                                                                        \
    ({                                                                                                                                                         \
        int p_ = (pos);                                                                                                                                        \
        int s_ = (size);                                                                                                                                       \
        int w_ = p_ % s_;                                                                                                                                      \
        (w_ < 0) ? w_ + s_ : w_;                                                                                                                               \
    })

#define WRAP_PLUS(pos, amount, size) WRAP((pos) + (amount), (size))
#define WRAP_MINUS(pos, amount, size) WRAP((pos) - (amount), (size))

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
#define WUXING_CHROMA_LUT                                                                                                                                      \
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
//#define AUDIO_DEVICE_CHANNELS 2

#define SRC_SAMPLE_RATE 22050
//#define AUDIO_DEVICE_SAMPLE_RATE 44100
#define ANALYSIS_SAMPLE_RATE 22050 //44100

#define SRC_BIT_DEPTH 16
//#define AUDIO_DEVICE_FORMAT 16
#define ANALYSIS_PCM16_UPPER_BOUND 32767.0f

#define ANALYSIS_WINDOW_SIZE_IN_FRAMES 1024 //2048
#define ANALYSIS_SPECTRUM_BIN_COUNT 512
#define ANALYSIS_WAVEFORM_SAMPLE_COUNT 512

#ifndef AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES
#define AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES 2048
#endif

#define Y_AXIS (Vector3){0.0f, 1.0f, 0.0f}
#define DEFAULT_SCALE (Vector3){1.0f, 1.0f, 1.0f}

#define AUDIO_ASSET_PATH(filename) AUDIO_ASSET_PATH_PREFIX filename
#define SHADERTOY_EXPERIMENT_22K_WAV AUDIO_ASSET_PATH("experiment_22k_mono_adpcm.wav")
#define SHADERTOY_ELECTRONEBULAE_22K_WAV AUDIO_ASSET_PATH("electronebl_22k_mono_adpcm.wav")
#define SHADERTOY_8BIT_22K_WAV AUDIO_ASSET_PATH("8bit_22k_mono_adpcm.wav")
#define SHADERTOY_GEOMETRIC_PERSON_22K_WAV AUDIO_ASSET_PATH("geometric_person_22k_mono_adpcm.wav")
#define SHADERTOY_TROPICAL_22K_WAV AUDIO_ASSET_PATH("tropical_22k_mono_adpcm.wav")
#define SHADERTOY_XTRACK_22K_WAV AUDIO_ASSET_PATH("xtrack_22k_mono_adpcm.wav")

#define RD_FONT "/rd/vga_rom_f16_0px_TIGHT.fnt" //TODO: nice 1KB...
#define FONT_SIZE 16.0f

typedef struct FFTComplex {
    float real;
    float imaginary;
} FFTComplex;

typedef struct FFTData {
    FFTComplex* work_buffer;
    float* smoothed_spectrum_magnitudes;
    unsigned int frame_pos;
    float (*raw_spectrum_magnitudes)[ANALYSIS_SPECTRUM_BIN_COUNT];
    float (*spectrum_levels)[ANALYSIS_SPECTRUM_BIN_COUNT];
    int history_frame_pos;
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
    int prev_log_sec;
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
            fftprof_profile->dur_min_ms = FMINF(fftprof_profile->dur_min_ms, fftprof_compute_ms);                                                              \
            fftprof_profile->dur_max_ms = FMAXF(fftprof_profile->dur_max_ms, fftprof_compute_ms);                                                              \
        }                                                                                                                                                      \
                                                                                                                                                               \
        int fftprof_elapsed_sec = (int)fftprof_elapsed_s;                                                                                                      \
        if (fftprof_elapsed_sec != fftprof_profile->prev_log_sec) {                                                                                            \
            if (fftprof_domain != NULL && fftprof_fft_data != NULL) {                                                                                          \
                char fftprof_timestamp[LOG_TIMESTAMP_SIZE];                                                                                                    \
                int fftprof_elapsed_ms = (int)(fftprof_elapsed_s * (float)MILLISECONDS_PER_SECOND);                                                            \
                float fftprof_avg_ms = fftprof_profile->dur_sum_ms / (float)fftprof_profile->run_count;                                                        \
                                                                                                                                                               \
                FORMAT_MMSSMMM(fftprof_timestamp, fftprof_elapsed_ms);                                                                                         \
                TraceLog(LOG_INFO,                                                                                                                             \
                         "[%s] [%s] frame_pos=%u; fft_run_count=%d; fft_run_dur_min_ms=%.3f; "                                                                 \
                         "fft_run_dur_max_ms=%.3f; fft_run_dur_avg_ms=%.3f",                                                                                   \
                         fftprof_timestamp,                                                                                                                    \
                         fftprof_domain,                                                                                                                       \
                         fftprof_fft_data->frame_pos,                                                                                                          \
                         fftprof_profile->run_count,                                                                                                           \
                         fftprof_profile->dur_min_ms,                                                                                                          \
                         fftprof_profile->dur_max_ms,                                                                                                          \
                         fftprof_avg_ms);                                                                                                                      \
            }                                                                                                                                                  \
                                                                                                                                                               \
            fftprof_profile->prev_log_sec = fftprof_elapsed_sec;                                                                                               \
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
static FFTData fft_data = {0};
static float analysis_window_samples[ANALYSIS_WINDOW_SIZE_IN_FRAMES] = {0};
static int16_t analysis_pcm16[ANALYSIS_WINDOW_SIZE_IN_FRAMES] = {0};

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

static inline void update_smoothing_and_spectrum_levels_bin(float* spectrum_levels, int bin, float real, float imaginary) {
    float linear_magnitude = SQRTF(real * real + imaginary * imaginary) / ANALYSIS_WINDOW_SIZE_IN_FRAMES;
    float smoothed_magnitude =
        ANALYSIS_SMOOTHING_TIME_CONSTANT * fft_data.smoothed_spectrum_magnitudes[bin] + (1.0f - ANALYSIS_SMOOTHING_TIME_CONSTANT) * linear_magnitude;
    fft_data.smoothed_spectrum_magnitudes[bin] = smoothed_magnitude;
    float db = LOGF(smoothed_magnitude) * ANALYSIS_DB_TO_LINEAR_SCALE;
    float level = CLAMP((db - ANALYSIS_MIN_DECIBELS) * ANALYSIS_INVERSE_DECIBEL_RANGE, 0.0f, 1.0f);
    spectrum_levels[bin] = level;
}

static inline void build_spectrum(void) {
    float* spectrum_levels = fft_data.spectrum_levels[fft_data.history_frame_pos];
    float* raw_spectrum_magnitudes = fft_data.raw_spectrum_magnitudes[fft_data.history_frame_pos];

    for (int i = 0; i < ANALYSIS_SPECTRUM_BIN_COUNT; i++) {
        float re = fft_data.work_buffer[i].real;
        float im = fft_data.work_buffer[i].imaginary;
        float magnitude = SQRTF(re * re + im * im) / ANALYSIS_WINDOW_SIZE_IN_FRAMES;
        raw_spectrum_magnitudes[i] = magnitude;
        update_smoothing_and_spectrum_levels_bin(spectrum_levels, i, re, im);
    }

    fft_data.history_frame_pos = WRAP_PLUS(fft_data.history_frame_pos, 1, ANALYSIS_FFT_HISTORY_FRAME_COUNT);
    fft_data.frame_pos++;
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
#define MESH_VERTEX_COUNT (LANE_COUNT * LANE_POINT_COUNT)
#define TERRAIN_TRIANGLE_COUNT (((LANE_COUNT - 1) * (LANE_POINT_COUNT - 1)) * 2)
#define FLAT_VERTEX_COUNT (TERRAIN_TRIANGLE_COUNT * 3)
#define DRAW_COLOR_CHANNEL_MAX 255

#define TOP (Vector3){0.0f, 2.0f, 0.0f}
#define MIDDLE (Vector3){0.0f}
#define BOTTOM (Vector3){0.0f, -1.0f, 0.0f}

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

static inline void smooth_lane(int lane) {
    for (int i = 0; i < LANE_POINT_COUNT; i++) {
        float sample_sum = 0.0f;
        int k = i * WAVEFORM_SAMPLES_PER_LANE_POINT;
        for (int j = 0; j < WAVEFORM_SAMPLES_PER_LANE_POINT; j++) {
            sample_sum += FABSF(analysis_window_samples[k + j]);
        }

        lane_point_values[lane][i] = (sample_sum / (float)WAVEFORM_SAMPLES_PER_LANE_POINT) * FRONT_LANE_SMOOTHING;
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

static inline void update_mesh_vertices(float* vertices, const float* lane_point_values, int point_count, float y_scale) {
    for (int i = 0; i < LANE_COUNT; i++) {
        float lane_offset = ((float)i - 0.5f * (float)(LANE_COUNT - 1)) * LANE_SPACING_SCALE;
        for (int j = 0; j < point_count; j++) {
            int k = (i * point_count + j) * 3;
            float x = (((float)j / (float)(point_count - 1)) - HALF_SPAN) * LINE_LENGTH_SCALE;
            float y = lane_point_values[i * point_count + j] * y_scale;
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

static inline void update_mesh_texcoords_smooth_scroll(
    int w, int h, float* texcoords, int point_count, int texels_per_quad, Vector2 scroll_direction, float scroll_speed, float time) {
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

static void update_mesh_vertices_flat(float* dst_vertices, const float* src_vertices, const unsigned short* indices, int flat_vertex_count) {
    for (int i = 0; i < flat_vertex_count; i++) {
        int src_index = indices[i];
        int src_vertex = src_index * 3;
        int dst_vertex = i * 3;
        dst_vertices[dst_vertex + 0] = src_vertices[src_vertex + 0];
        dst_vertices[dst_vertex + 1] = src_vertices[src_vertex + 1];
        dst_vertices[dst_vertex + 2] = src_vertices[src_vertex + 2];
    }
}

static void expand_mesh_colors_flat(Color* dst_colors, const Color* src_colors, const unsigned short* indices, int flat_vertex_count) {
    for (int i = 0; i < flat_vertex_count; i++) {
        int src_index = indices[i];
        dst_colors[i] = src_colors[src_index];
    }
}

static void update_mesh_normals_flat(float* normals, const float* vertices, int triangle_count) {
    for (int i = 0; i < triangle_count; i++) {
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

static void expand_mesh_normals_flat(float* dst_normals, const float* src_normals, const unsigned short* indices, int flat_vertex_count) {
    for (int i = 0; i < flat_vertex_count; i++) {
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

static void build_mesh_flat(Mesh* dst_mesh, const float* flat_vertices, const float* src_normals, const Color* src_colors, int flat_vertex_count) {
    MEMCPY4(dst_mesh->vertices, flat_vertices, sizeof(float) * flat_vertex_count * 3);
    MEMCPY4(dst_mesh->normals, src_normals, sizeof(float) * flat_vertex_count * 3);
    MEMCPY(dst_mesh->colors, src_colors, sizeof(Color) * flat_vertex_count);
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

#define ADAPTIVE_ONSET_RATE 0.020f
#define ADAPTIVE_ONSET_GATE_DEVIATION_SCALE 0.75f
#define ADAPTIVE_ONSET_RANGE_DEVIATION_SCALE 3.00f
#define GLOBAL_ADAPTIVE_GATE_SMOOTHING_FACTOR 0.08f
#define GLOBAL_ADAPTIVE_GATE_FLOOR_DEVIATION_SCALE 0.75f
#define GLOBAL_ADAPTIVE_GATE_CEILING_DEVIATION_SCALE 1.25f
#define ONSET_DECAY_RATE 0.35f // 0.10f
#define ONSET_DELAY_FRAMES 1
#define ONSET_ENVELOPE_RADIUS 2
#define LIGHT0_DIFFUSE_MAX 1.5f

static float onset_gate = 0.0f;
static float onset_gate_history[ANALYSIS_FFT_HISTORY_FRAME_COUNT] = {0};
static float adaptive_onset_mean = 0.0f;
static float adaptive_onset_deviation = 0.0f;
static float adaptive_chroma_mask_mean = 0.0f;
static float adaptive_chroma_mask_deviation = 0.0f;
static int adaptive_chroma_mask_ready = 0;
static float light0_diffuse[4] = {0.0f, 0.0f, 0.0f, 1.0f};
static Vector3 light0_pos = {1.330f, 1.345f, -1.418f};

static inline void reset_shared_adaptive_audio_state(void) {
    MEMSET(onset_gate_history, 0, sizeof(onset_gate_history));
    onset_gate = 0.0f;
    adaptive_onset_mean = 0.0f;
    adaptive_onset_deviation = 0.0f;
    adaptive_chroma_mask_mean = 0.0f;
    adaptive_chroma_mask_deviation = 0.0f;
    adaptive_chroma_mask_ready = 0;
}

static void update_onset_gate_fft(FFTData* fft_data, int use_raw) {
    // librosa onset_strength convention:
    // https://librosa.org/doc/main/generated/librosa.onset.onset_strength.html
    // TODO: onset_detect would be a later peak-pick stage is more complicated...
    // https://librosa.org/doc/main/generated/librosa.onset.onset_detect.html
    int cur_history_frame_pos = WRAP_MINUS(fft_data->history_frame_pos, 1, ANALYSIS_FFT_HISTORY_FRAME_COUNT);
    if (fft_data->frame_pos <= ONSET_DELAY_FRAMES) {
        onset_gate_history[cur_history_frame_pos] = onset_gate;
        return;
    }
    int delay_history_frame_pos = WRAP_MINUS(fft_data->history_frame_pos, 1 + ONSET_DELAY_FRAMES, ANALYSIS_FFT_HISTORY_FRAME_COUNT);
    float* spectrum_levels = fft_data->spectrum_levels[cur_history_frame_pos];
    float* delay_spectrum_levels = fft_data->spectrum_levels[delay_history_frame_pos];
    float* raw_spectrum_magnitudes = fft_data->raw_spectrum_magnitudes[cur_history_frame_pos];
    float* delay_raw_spectrum_magnitudes = fft_data->raw_spectrum_magnitudes[delay_history_frame_pos];
    float flux_sum = 0.0f;
    for (int i = 0; i < ANALYSIS_SPECTRUM_BIN_COUNT; i++) {
        float cur = spectrum_levels[i];
        float ref = delay_spectrum_levels[i];
        if (use_raw) {
            cur = LOGF(raw_spectrum_magnitudes[i]) * ANALYSIS_DB_TO_LINEAR_SCALE;
            ref = LOGF(delay_raw_spectrum_magnitudes[i]) * ANALYSIS_DB_TO_LINEAR_SCALE;
        }
        int j_min = MAXI(i - ONSET_ENVELOPE_RADIUS, 0);
        int j_max = MINI(i + ONSET_ENVELOPE_RADIUS, ANALYSIS_SPECTRUM_BIN_COUNT - 1);
        for (int j = j_min; j <= j_max; j++) {
            float ref_candidate = delay_spectrum_levels[j];
            if (use_raw) {
                ref_candidate = LOGF(delay_raw_spectrum_magnitudes[j]) * ANALYSIS_DB_TO_LINEAR_SCALE;
            }
            ref = FMAXF(ref, ref_candidate);
        }
        flux_sum += FMAXF(cur - ref, 0.0f); // librosa onset_strength: max(0, S[f, t] - ref[f, t - lag])
    }
    float onset_flux = flux_sum / (float)ANALYSIS_SPECTRUM_BIN_COUNT;
    float adaptive_onset_delta = FABSF(onset_flux - adaptive_onset_mean);
    adaptive_onset_mean = LERP(adaptive_onset_mean, onset_flux, ADAPTIVE_ONSET_RATE);
    adaptive_onset_deviation = LERP(adaptive_onset_deviation, adaptive_onset_delta, ADAPTIVE_ONSET_RATE);
    float adaptive_onset_floor = adaptive_onset_mean + adaptive_onset_deviation * ADAPTIVE_ONSET_GATE_DEVIATION_SCALE;
    float adaptive_onset_range = adaptive_onset_deviation * ADAPTIVE_ONSET_RANGE_DEVIATION_SCALE;
    float onset_gate_target = CLAMP((onset_flux - adaptive_onset_floor) / adaptive_onset_range, 0.0f, 1.0f);
    if (onset_gate_target > onset_gate) {
        onset_gate = onset_gate_target;
    } else {
        onset_gate = LERP(onset_gate, onset_gate_target, ONSET_DECAY_RATE);
    }
    onset_gate = CLAMP(onset_gate, 0.0f, 1.0f);
    onset_gate_history[cur_history_frame_pos] = onset_gate;
}

static void update_diffuse_strength(void) {
    float light0_diffuse_strength = LIGHT0_DIFFUSE_MAX * onset_gate;
    light0_diffuse[0] = light0_diffuse_strength;
    light0_diffuse[1] = light0_diffuse_strength;
    light0_diffuse[2] = light0_diffuse_strength;
    light0_diffuse[3] = 1.0f;
}

#define LIGHT_STRAFE_DEADZONE 0.20f
#define LIGHT_STRAFE_RANGE 2.5f
#define LIGHT_STRAFE_RESPONSE 8.0f
#define LIGHT_REST_BOB_AMP 0.50f
#define LIGHT_SCENE_X_HALF (HALF_SPAN * LINE_LENGTH_SCALE)
#define LIGHT_SCENE_Z_HALF (0.5f * (float)(LANE_COUNT - 1) * LANE_SPACING_SCALE)

static inline Vector3 compute_buoy_rest_pos(const float* vertices, int point_count, Vector3 anchor_pos) {
    static float cur_surface_y = 0.0f;
    static float prev_surface_y = 0.0f;
    static float rest_bob_y = LIGHT_REST_BOB_AMP;
    static float rest_bob_vel = 0.0f;
    static int is_ready = 0;
    const int center_lane = LANE_COUNT / 2;
    const int center_point[2] = {(point_count - 1) / 2, point_count / 2};
    const int center_vertex_offset[2] = {
        (center_lane * point_count + center_point[0]) * 3,
        (center_lane * point_count + center_point[1]) * 3,
    };
    const float surface_follow_rate = 3.75f;
    const float surface_impulse_gain = 3.25f;
    const float bob_stiffness = 18.0f;
    const float bob_friction = 7.25f;
    const float bob_ceiling = 0.30f;
    Vector3 center_surface_pos =
        Vector3Lerp((Vector3){vertices[center_vertex_offset[0] + 0], vertices[center_vertex_offset[0] + 1], vertices[center_vertex_offset[0] + 2]},
                    (Vector3){vertices[center_vertex_offset[1] + 0], vertices[center_vertex_offset[1] + 1], vertices[center_vertex_offset[1] + 2]},
                    0.5f);
    if (!is_ready) {
        cur_surface_y = center_surface_pos.y;
        prev_surface_y = center_surface_pos.y;
        rest_bob_y = LIGHT_REST_BOB_AMP;
        rest_bob_vel = 0.0f;
        is_ready = 1;
    }
    float dt = CLAMP(GetFrameTime(), 0.0f, 1.0f / 30.0f);
    cur_surface_y = LERP(cur_surface_y, center_surface_pos.y, CLAMP(dt * surface_follow_rate, 0.0f, 1.0f));
    rest_bob_vel += (cur_surface_y - prev_surface_y) * surface_impulse_gain;
    rest_bob_vel += ((LIGHT_REST_BOB_AMP - rest_bob_y) * bob_stiffness - rest_bob_vel * bob_friction) * dt;
    rest_bob_y = CLAMP(rest_bob_y + rest_bob_vel * dt, LIGHT_REST_BOB_AMP - bob_ceiling, LIGHT_REST_BOB_AMP + bob_ceiling);
    prev_surface_y = cur_surface_y;
    return (Vector3){anchor_pos.x + center_surface_pos.x, anchor_pos.y + cur_surface_y + rest_bob_y, anchor_pos.z + center_surface_pos.z};
}

static inline void update_light_camera_strafe(const Camera3D* camera, Vector3 rest_pos, BoundingBox scene_bbox) {
    float strafe_axis_x =
        (FABSF(GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X)) < LIGHT_STRAFE_DEADZONE) ? 0.0f : GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
    float strafe_axis_y =
        (FABSF(GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y)) < LIGHT_STRAFE_DEADZONE) ? 0.0f : GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);

    Vector3 cam_forward = Vector3Normalize(Vector3Subtract(camera->target, camera->position));
    Vector3 cam_right = Vector3Normalize(Vector3CrossProduct(cam_forward, camera->up));
    Vector3 cam_up = Vector3CrossProduct(cam_right, cam_forward);
    Vector3 strafe_pos_offset =
        Vector3Add(Vector3Scale(cam_right, strafe_axis_x * LIGHT_STRAFE_RANGE), Vector3Scale(cam_up, -strafe_axis_y * LIGHT_STRAFE_RANGE));
    Vector3 target_pos = Vector3Add(rest_pos, strafe_pos_offset);

    float dt = CLAMP(GetFrameTime(), 0.0f, 1.0f / 30.0f);
    float strafe_lerp = CLAMP(dt * LIGHT_STRAFE_RESPONSE, 0.0f, 1.0f);
    light0_pos = Vector3Lerp(light0_pos, target_pos, strafe_lerp);

    light0_pos.x = CLAMP(light0_pos.x, scene_bbox.min.x, scene_bbox.max.x);
    light0_pos.y = CLAMP(light0_pos.y, scene_bbox.min.y, scene_bbox.max.y);
    light0_pos.z = CLAMP(light0_pos.z, scene_bbox.min.z, scene_bbox.max.z);
}

#define LIGHT0_CAPSULE_CROWN_COUNT 3
#define LIGHT0_CAPSULE_SEGMENT_COUNT 12
#define LIGHT0_LANTERN_RADIUS 0.33f
#define LIGHT0_LANTERN_RIB_PAIR_COUNT 4

//TODO: based off of capsule gen mesh but with a silly onset driven spin...
static inline void draw_lantern(Vector3 pos) {
    Color lantern_color = SUNFLOWER;
    static float spin_phase = 0.0f;
    static float spin_vel = 0.03f;
    static float spin_drive = 0.0f;
    static float prev_onset = 0.0f;
    float dt = CLAMP(GetFrameTime(), 0.0f, 1.0f / 30.0f);
    float onset = CLAMP((onset_gate - 0.18f) / 0.82f, 0.0f, 1.0f);
    float onset_burst = FMAXF(onset - prev_onset, 0.0f);
    prev_onset = onset;
    float lantern_radius = LIGHT0_LANTERN_RADIUS * 0.94f;
    float spin_idle = 0.035f / lantern_radius;
    float spin_max = 3.40f / lantern_radius;
    float spin_accel = 52.0f / lantern_radius;
    float spin_drag = 3.25f;
    float spin_drive_drag = 13.0f;
    float onset_impulse = SQRTF(onset_burst) * (0.45f + 0.55f * onset);
    spin_drive = FMAXF(spin_drive * EXPF(-spin_drive_drag * dt), onset_impulse);
    spin_vel += spin_drive * spin_accel * dt;
    spin_vel = FMINF(spin_vel, spin_max);
    spin_vel = spin_idle + (spin_vel - spin_idle) * EXPF(-spin_drag * dt);
    spin_phase -= spin_vel * dt;
    if (spin_phase < -4096.0f * PI)
        spin_phase += 4096.0f * PI;
    if (spin_phase > 4096.0f * PI)
        spin_phase -= 4096.0f * PI;
    float body_half_height = LIGHT0_LANTERN_RADIUS * 0.38f;
    int rib_count = LIGHT0_LANTERN_RIB_PAIR_COUNT * 2;
    float rib_slice_angle_step = (2.0f * PI) / (float)rib_count;
    float crown_arc_angle_step = (0.5f * PI) / (float)rib_count;
    float spin_phase_offset = spin_phase;
    int crown_index = CLAMP((int)(0.82f * (float)rib_count), 1, rib_count - 1);
    float crown_angle = crown_arc_angle_step * (float)crown_index;
#define LANTERN_VERTEX(_cap_center_y, _cap_sign, _crown_sin, _crown_cos, _rib_sin, _rib_cos)                                                                   \
    ((Vector3){pos.x + (_rib_sin) * (_crown_cos) * lantern_radius,                                                                                             \
               pos.y + (_cap_center_y) + (_cap_sign) * (_crown_sin) * lantern_radius,                                                                          \
               pos.z + (_rib_cos) * (_crown_cos) * lantern_radius})
    for (int i = 0; i < rib_count; i++) {
        float rib_phase = ((2.0f * PI * (float)i) / (float)rib_count) + spin_phase_offset;
        float rib_slice_sin = SINF(rib_phase);
        float rib_slice_cos = COSF(rib_phase);
        DrawLine3D((Vector3){pos.x + rib_slice_sin * lantern_radius, pos.y + body_half_height, pos.z + rib_slice_cos * lantern_radius},
                   (Vector3){pos.x + rib_slice_sin * lantern_radius, pos.y - body_half_height, pos.z + rib_slice_cos * lantern_radius},
                   lantern_color);
    }
    for (int i = 0; i < rib_count; i++) {
        float rib_slice_phase = rib_slice_angle_step * (float)i + spin_phase_offset;
        float rib_slice_sin = SINF(rib_slice_phase);
        float rib_slice_cos = COSF(rib_slice_phase);
        for (int k = 0; k < crown_index; k++) {
            float crown_arc_angle_0 = crown_arc_angle_step * (float)k;
            float crown_arc_angle_1 = crown_arc_angle_step * (float)(k + 1);
            float crown_arc_sin_0 = SINF(crown_arc_angle_0);
            float crown_arc_cos_0 = COSF(crown_arc_angle_0);
            float crown_arc_sin_1 = SINF(crown_arc_angle_1);
            float crown_arc_cos_1 = COSF(crown_arc_angle_1);
            DrawLine3D(LANTERN_VERTEX(-body_half_height, -1.0f, crown_arc_sin_0, crown_arc_cos_0, rib_slice_sin, rib_slice_cos),
                       LANTERN_VERTEX(-body_half_height, -1.0f, crown_arc_sin_1, crown_arc_cos_1, rib_slice_sin, rib_slice_cos),
                       lantern_color);
            DrawLine3D(LANTERN_VERTEX(body_half_height, 1.0f, crown_arc_sin_0, crown_arc_cos_0, rib_slice_sin, rib_slice_cos),
                       LANTERN_VERTEX(body_half_height, 1.0f, crown_arc_sin_1, crown_arc_cos_1, rib_slice_sin, rib_slice_cos),
                       lantern_color);
        }
    }
    for (int i = 0; i < rib_count; i++) {
        float crown_phase_0 = rib_slice_angle_step * (float)i + spin_phase_offset;
        float crown_phase_1 = rib_slice_angle_step * (float)(i + 1) + spin_phase_offset;
        float crown_sin = SINF(crown_angle);
        float crown_cos = COSF(crown_angle);
        DrawLine3D(LANTERN_VERTEX(-body_half_height, -1.0f, crown_sin, crown_cos, SINF(crown_phase_0), COSF(crown_phase_0)),
                   LANTERN_VERTEX(-body_half_height, -1.0f, crown_sin, crown_cos, SINF(crown_phase_1), COSF(crown_phase_1)),
                   lantern_color);
        DrawLine3D(LANTERN_VERTEX(body_half_height, 1.0f, crown_sin, crown_cos, SINF(crown_phase_0), COSF(crown_phase_0)),
                   LANTERN_VERTEX(body_half_height, 1.0f, crown_sin, crown_cos, SINF(crown_phase_1), COSF(crown_phase_1)),
                   lantern_color);
    }
#undef LANTERN_VERTEX
}

static inline void draw_lantern_glow(Vector3 pos) {
    Color glow_color = SUNFLOWER;
    static float glow = 0.0f;
    float dt = CLAMP(GetFrameTime(), 0.0f, 1.0f / 30.0f);
    float onset_glow_target = CLAMP(onset_gate, 0.0f, 1.0f);
    glow = LERP(glow, onset_glow_target, CLAMP(dt * (onset_glow_target > glow ? 5.0f : 0.85f), 0.0f, 1.0f));
    float g = glow * glow;
    float R = LIGHT0_LANTERN_RADIUS * (1.0f + g * 1.15f);
    float r0 = R * 0.94f, r1 = R * 0.40f;
    float y0 = R * 0.38f, y1 = R * 0.18f;
    Vector3 a0 = {pos.x, pos.y - y0, pos.z}, b0 = {pos.x, pos.y + y0, pos.z};
    Vector3 a1 = {pos.x, pos.y - y1, pos.z}, b1 = {pos.x, pos.y + y1, pos.z};
    rlDisableDepthTest();
    glow_color.a = (unsigned char)(14.0f + g * 34.0f);
    DrawCapsule(a0, b0, r0, LIGHT0_CAPSULE_SEGMENT_COUNT, LIGHT0_CAPSULE_CROWN_COUNT, glow_color);
    glow_color.a = (unsigned char)(34.0f + g * 72.0f);
    DrawCapsule(a1, b1, r1, LIGHT0_CAPSULE_SEGMENT_COUNT / 2, LIGHT0_CAPSULE_CROWN_COUNT / 2 + 1, glow_color);
    rlEnableDepthTest();
}

#define CAMERA_FOVY_MIN 0.1f
#define CAMERA_FOVY_MAX 10.0f
#define CAMERA_FOVY_VELOCITY 6.0f
#define CAMERA_ORBIT_VELOCITY 2.0f
#define CAMERA_PITCH_MIN (-PI / 2.0f + 0.1f)
#define CAMERA_PITCH_MAX (PI / 2.0f - 0.1f)
#define BUTTON_DOWN_NAV_INITIAL_DELAY_SECONDS 0.35f
// TODO: hardmode-> find the exact lane advance cadence for this value, lock it to playback rate a default,
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

    pitch = CLAMP(pitch, CAMERA_PITCH_MIN, CAMERA_PITCH_MAX);
    fovy = CLAMP(fovy, CAMERA_FOVY_MIN, CAMERA_FOVY_MAX);
    camera->position.x = camera->target.x + orbit_radius * cosf(pitch) * cosf(yaw);
    camera->position.y = camera->target.y + orbit_radius * sinf(pitch);
    camera->position.z = camera->target.z + orbit_radius * cosf(pitch) * sinf(yaw);
    camera->fovy = fovy;
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

#define YAMAHA_ADPCM_MIN_STEP_SIZE 127
#define YAMAHA_ADPCM_MAX_STEP_SIZE 24576
#define YAMAHA_ADPCM_READ_BUFFER_SIZE 2048
#define ADPCM_CLAMP(value, min, max) ((int)CLAMP((float)(value), (float)(min), (float)(max)))
#define ADPCM_CLAMP_PCM16(value) ((int16_t)ADPCM_CLAMP((value), INT16_MIN, INT16_MAX))
#define ADPCM_CLAMP_STEP_SIZE(value) ((int16_t)ADPCM_CLAMP((value), YAMAHA_ADPCM_MIN_STEP_SIZE, YAMAHA_ADPCM_MAX_STEP_SIZE))
static const int YAMAHA_ADPCM_STEP_TABLE[8] = {230, 230, 230, 230, 307, 409, 512, 614};

static int src_file = 0;
static int src_file_data_offset = 0;
static int src_file_chunk_size = 0;
static int src_file_fact_frame_count = 0;
static int src_file_format_tag = 0;
static int adpcm_decode_frame = 0;
static int16_t adpcm_decode_history = 0;
static int16_t adpcm_step_size = YAMAHA_ADPCM_MIN_STEP_SIZE;
static unsigned char adpcm_read_buffer[YAMAHA_ADPCM_READ_BUFFER_SIZE] = {0};
static int adpcm_start_pos = -1;
static int adpcm_read_count = 0;
static int adpcm_checkpoint_count = 0;
static int adpcm_checkpoint_filled_count = 0;
static int16_t* adpcm_checkpoint_history = NULL;
static int16_t* adpcm_checkpoint_step_size = NULL;

// TODO: double check this verbosity against any much more tight and canonical ways to do this
// e.g. currently all wavs are preprocessed to get the same header and tags via ffmpeg, so this owuld only be relevant for if later arbitrary wav files are handled, but adpcm...
static inline int wav_probe_data_chunk(void) {
    src_file_data_offset = 0;
    src_file_chunk_size = 0;
    src_file_fact_frame_count = 0;
    src_file_format_tag = 0;
    uint32_t header[3];
    fs_seek(src_file, 0, SEEK_SET);
    if (fs_read(src_file, header, 12) != 12) {
        return 0;
    }
    if (header[0] != 0x46464952 || header[2] != 0x45564157) {
        return 0;
    }
    unsigned int file_size = (unsigned int)fs_total(src_file);
    unsigned int pos = 12;
    while (pos + 8 <= file_size) {
        uint32_t chunk[2];
        fs_seek(src_file, pos, SEEK_SET);
        if (fs_read(src_file, chunk, 8) != 8) {
            return 0;
        }
        int chunk_data_pos = pos + 8;
        if (chunk_data_pos + chunk[1] > file_size) {
            return 0;
        }
        if (chunk[0] == 0x20746D66) {
            fs_seek(src_file, chunk_data_pos, SEEK_SET);
            fs_read(src_file, &src_file_format_tag, sizeof(src_file_format_tag));
        } else if (chunk[0] == 0x74636166) {
            fs_seek(src_file, chunk_data_pos, SEEK_SET);
            fs_read(src_file, &src_file_fact_frame_count, sizeof(src_file_fact_frame_count));
        } else if (chunk[0] == 0x61746164) {
            src_file_data_offset = chunk_data_pos;
            src_file_chunk_size = chunk[1];
        }
        pos = chunk_data_pos + chunk[1] + (chunk[1] & 1);
    }
    return src_file_format_tag == 0x0020 && src_file_data_offset != 0 && src_file_chunk_size != 0;
}

// TODO: still hard to tell if zeroing and re-initing everything can even be centralized with how much state jgulling is happening now... oh dear
static inline void reset_adpcm_decoder(void) {
    adpcm_decode_frame = 0;
    adpcm_decode_history = 0;
    adpcm_step_size = YAMAHA_ADPCM_MIN_STEP_SIZE;
    adpcm_start_pos = -1;
    adpcm_read_count = 0;
}

static inline unsigned char adpcm_read_byte(int byte_index) {
    if (byte_index < adpcm_start_pos || byte_index >= adpcm_start_pos + adpcm_read_count) {
        fs_seek(src_file, src_file_data_offset + byte_index, SEEK_SET);
        adpcm_start_pos = byte_index;
        adpcm_read_count = fs_read(src_file, adpcm_read_buffer, YAMAHA_ADPCM_READ_BUFFER_SIZE);
        if (adpcm_read_count <= 0) {
            adpcm_start_pos = -1;
            adpcm_read_count = 0;
            return 0;
        }
    }
    return adpcm_read_buffer[byte_index - adpcm_start_pos];
}

static inline int16_t adpcm_decode(unsigned char nibble) {
    adpcm_decode_history = (int16_t)((adpcm_decode_history * 254) / 256);
    int sign = nibble & 8;
    int delta = nibble & 7;
    int diff = ((1 + (delta << 1)) * adpcm_step_size) >> 3;
    int sample = adpcm_decode_history + (sign ? -diff : diff);
    int next_step_size = (YAMAHA_ADPCM_STEP_TABLE[delta] * adpcm_step_size) >> 8;
    adpcm_decode_history = ADPCM_CLAMP_PCM16(sample);
    adpcm_step_size = ADPCM_CLAMP_STEP_SIZE(next_step_size);
    return adpcm_decode_history;
}

static inline void build_adpcm_checkpoints(void) {
    adpcm_checkpoint_count = (wave.frameCount + AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES - 1) / AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES;
    adpcm_checkpoint_filled_count = 1;
    adpcm_checkpoint_history = RL_CALLOC(adpcm_checkpoint_count, sizeof(int16_t));
    adpcm_checkpoint_step_size = RL_CALLOC(adpcm_checkpoint_count, sizeof(int16_t));
    adpcm_checkpoint_history[0] = 0;
    adpcm_checkpoint_step_size[0] = YAMAHA_ADPCM_MIN_STEP_SIZE;
    reset_adpcm_decoder(); // TODO: again... clearly im not sure..
}

static inline void adpcm_save_checkpoint(void) {
    if ((adpcm_decode_frame % AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES) != 0) {
        return;
    }
    int checkpoint_index = adpcm_decode_frame / AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES;
    if (checkpoint_index >= adpcm_checkpoint_count) {
        return;
    }
    adpcm_checkpoint_history[checkpoint_index] = adpcm_decode_history;
    adpcm_checkpoint_step_size[checkpoint_index] = adpcm_step_size;
    adpcm_checkpoint_filled_count = MAXI(adpcm_checkpoint_filled_count, checkpoint_index + 1);
}

static inline void fffftt_file_io_seek_read(int16_t* dst, int pos, int frame_count) {
    int src_pos = WRAP(pos, wave.frameCount);
    int write_count = 0;
    while (write_count < frame_count) {
        if (src_pos < adpcm_decode_frame) {
            int checkpoint_index = src_pos / AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES;
            checkpoint_index = MINI(checkpoint_index, adpcm_checkpoint_filled_count - 1);
            adpcm_decode_frame = checkpoint_index * AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES;
            adpcm_decode_history = adpcm_checkpoint_history[checkpoint_index];
            adpcm_step_size = adpcm_checkpoint_step_size[checkpoint_index];
            adpcm_start_pos = -1;
            adpcm_read_count = 0;
        }
        while (adpcm_decode_frame < src_pos) {
            unsigned char byte = adpcm_read_byte(adpcm_decode_frame >> 1);
            unsigned char nibble = (adpcm_decode_frame & 1) ? (unsigned char)(byte >> 4) : (unsigned char)(byte & 0x0F);
            adpcm_decode(nibble);
            adpcm_decode_frame++;
            adpcm_save_checkpoint();
        }
        unsigned char byte = adpcm_read_byte(adpcm_decode_frame >> 1);
        unsigned char nibble = (adpcm_decode_frame & 1) ? (unsigned char)(byte >> 4) : (unsigned char)(byte & 0x0F);
        dst[write_count++] = adpcm_decode(nibble);
        adpcm_decode_frame++;
        adpcm_save_checkpoint();
        src_pos = WRAP_PLUS(src_pos, 1, wave.frameCount);
    }
}

//TODO: this is synchronous and thus very poor with BBA transport, but i am interested in learning how to follow
// through with this even if only for dev environment context, just to avoid fully resident wav_pcm, as a way to learn
//  about adpcm and other tricks in dreamcast auduio domain... it feels very important to learn this stuff.
static inline int fffftt_audio_process(int16_t* audio_period_pcm16) {
    if (is_paused || !IsAudioStreamProcessed(audio_stream)) {
        return 0;
    }
    fffftt_file_io_seek_read(audio_period_pcm16, wave_cursor, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
    wave_cursor = WRAP_PLUS(wave_cursor, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES, wave.frameCount);
    UpdateAudioStream(audio_stream, audio_period_pcm16, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
    for (int i = 0; i < ANALYSIS_WINDOW_SIZE_IN_FRAMES; i++) {
        analysis_window_samples[i] = (float)audio_period_pcm16[i] / ANALYSIS_PCM16_UPPER_BOUND;
    }
    return 1;
}

static inline void fffftt_audio_drain(void) {
    for (int i = 0; i < MAX_DRAIN_CHUNK_COUNT; i++) {
        while (!IsAudioStreamProcessed(audio_stream)) {
            /* KEEP DRAINING! */
        }
        UpdateAudioStream(audio_stream, drain_chunk_samples, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
    }
}

static inline void fffftt_inspection_fill_analysis_window(int cursor) {
    fffftt_file_io_seek_read(analysis_pcm16, cursor, ANALYSIS_WINDOW_SIZE_IN_FRAMES);
    for (int i = 0; i < ANALYSIS_WINDOW_SIZE_IN_FRAMES; i++) {
        analysis_window_samples[i] = (float)analysis_pcm16[i] / ANALYSIS_PCM16_UPPER_BOUND;
    }
}

static inline float fffftt_accumulate_sound_envelope(int pos, int frame_count) {
    int16_t envelope_pcm16[AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES];
    int read_count = MINI(frame_count, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
    fffftt_file_io_seek_read(envelope_pcm16, pos, read_count);

    float sum = 0.0f;
    for (int i = 0; i < read_count; i++) {
        sum += FABSF((float)envelope_pcm16[i] / ANALYSIS_PCM16_UPPER_BOUND);
    }
    return sum;
}

#define FORWARD 1
#define BACKWARD -1

#define SHADERTOY_EXPERIMENT 0
#define SHADERTOY_ELECTRONEBULAE 1
#define SHADERTOY_8BIT 2
#define SHADERTOY_GEOMETRIC_PERSON 3
#define SHADERTOY_TROPICAL 4
#define SHADERTOY_XTRACK 5
#define AUDIO_TRACK_COUNT 6

#define AUDIO_TRACK_PATH(track_index)                                                                                                                          \
    ((track_index) == SHADERTOY_EXPERIMENT         ? SHADERTOY_EXPERIMENT_22K_WAV                                                                              \
     : (track_index) == SHADERTOY_ELECTRONEBULAE   ? SHADERTOY_ELECTRONEBULAE_22K_WAV                                                                          \
     : (track_index) == SHADERTOY_8BIT             ? SHADERTOY_8BIT_22K_WAV                                                                                    \
     : (track_index) == SHADERTOY_GEOMETRIC_PERSON ? SHADERTOY_GEOMETRIC_PERSON_22K_WAV                                                                        \
     : (track_index) == SHADERTOY_TROPICAL         ? SHADERTOY_TROPICAL_22K_WAV                                                                                \
                                                   : SHADERTOY_XTRACK_22K_WAV)
static int audio_track_index = SHADERTOY_EXPERIMENT;

static inline void unload_audio_track(void) {
    RL_FREE(adpcm_checkpoint_history);
    RL_FREE(adpcm_checkpoint_step_size);
    adpcm_checkpoint_count = 0;
    adpcm_checkpoint_filled_count = 0;
    if (src_file) {
        fs_close(src_file);
    }
    src_file = 0;
    src_file_data_offset = 0;
    src_file_chunk_size = 0;
    src_file_fact_frame_count = 0;
    src_file_format_tag = 0;
    reset_adpcm_decoder();
    wave = (Wave){0};
}

static inline void set_audio_track(int track_index) {
    if (src_file) {
        unload_audio_track();
    }
    audio_track_index = track_index;
    src_file = fs_open(AUDIO_TRACK_PATH(audio_track_index), O_RDONLY);
    wav_probe_data_chunk();
    wave = (Wave){
        .frameCount = src_file_fact_frame_count ? src_file_fact_frame_count : src_file_chunk_size * 2,
        .sampleRate = SRC_SAMPLE_RATE,
        .sampleSize = SRC_BIT_DEPTH,
        .channels = SRC_CHANNELS,
        .data = NULL,
    };
    reset_adpcm_decoder();
    build_adpcm_checkpoints();
    wave_cursor = 0;
}

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

    bool was_paused = is_paused;
    audio_track_index += dir;
    if (audio_track_index < 0) {
        audio_track_index = AUDIO_TRACK_COUNT - 1;
    } else if (audio_track_index >= AUDIO_TRACK_COUNT) {
        audio_track_index = 0;
    }

    set_audio_track(audio_track_index);
    wave_cursor = 0;
    paused_wave_cursor = 0;
    seek_delta_chunks = 0;
    is_paused = false;
    reset_sticky_nav();
    if (was_paused) {
        PlayAudioStream(audio_stream);
    }
}

static void rebase_smooth_envelope_at_wave_cursor(void) {
    for (int i = 0; i < LANE_COUNT; i++) {
        int start_frame = WRAP_MINUS(wave_cursor, i * AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES, wave.frameCount);
        fffftt_inspection_fill_analysis_window(start_frame);
        smooth_lane(i);
    }
    fffftt_inspection_fill_analysis_window(wave_cursor);
}

#define WAVE_CURSOR_BLINK_RATE 8.0f
static Model* wave_cursor_model = NULL;
static Texture2D wave_cursor_texture = {0};

static inline Color fffftt_wave_cursor_blink_color(void) {
    float blink = SINF((float)GetTime() * WAVE_CURSOR_BLINK_RATE);
    return CLITERAL(Color){
        (unsigned char)((float)NEON_CARROT.r * (0.725f + 0.275f * blink)),
        (unsigned char)((float)NEON_CARROT.g * (0.725f + 0.275f * blink)),
        (unsigned char)((float)NEON_CARROT.b * (0.725f + 0.275f * blink)),
        (unsigned char)(175.5f + 79.5f * blink),
    };
}

//TODO: this sticks ou like a sore thumb compared to when we just did it in the envelope code itself i think? or at least in parity with the terrains
static inline void fffftt_inspection_step_sound_envelope(int dir) {
    if (dir == BACKWARD) {
        wave_cursor = WRAP_MINUS(wave_cursor, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES, wave.frameCount);
        seek_delta_chunks--;
        for (int lane = 0; lane < LANE_COUNT - 1; lane++) {
            for (int point = 0; point < LANE_POINT_COUNT; point++) {
                lane_point_values[lane][point] = lane_point_values[lane + 1][point];
            }
        }
        int tail_cursor = WRAP_MINUS(wave_cursor, (LANE_COUNT - 1) * AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES, wave.frameCount);
        fffftt_inspection_fill_analysis_window(tail_cursor);
        smooth_lane(LANE_COUNT - 1);
        fffftt_inspection_fill_analysis_window(wave_cursor);
    } else if (dir == FORWARD) {
        wave_cursor = WRAP_PLUS(wave_cursor, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES, wave.frameCount);
        seek_delta_chunks++;
        for (int lane = LANE_COUNT - 1; lane > 0; lane--) {
            for (int point = 0; point < LANE_POINT_COUNT; point++) {
                lane_point_values[lane][point] = lane_point_values[lane - 1][point];
            }
        }
        fffftt_inspection_fill_analysis_window(wave_cursor);
        smooth_lane(0);
    }
}

static void draw_paused_wave_cursor_lane_marker(int point_count) {
    int mesh_vertex_count = LANE_COUNT * point_count;
    Color wave_cursor_colors[mesh_vertex_count];
    int lane_index = seek_delta_chunks;
    float z_offset = 0.0f;
    Color blink_color = fffftt_wave_cursor_blink_color();
    if (lane_index < 0) {
        lane_index = 0;
        z_offset = -0.75f * LANE_SPACING_SCALE;
    } else if (lane_index >= LANE_COUNT) {
        lane_index = LANE_COUNT - 1;
        z_offset = 0.75f * LANE_SPACING_SCALE;
    }
    for (int i = 0; i < mesh_vertex_count; i++) {
        wave_cursor_colors[i] = BLANK;
    }
    for (int i = 0; i < point_count; i++) {
        wave_cursor_colors[lane_index * point_count + i] = blink_color;
    }

    unsigned char* saved_colors = wave_cursor_model->meshes[0].colors;
    int saved_texture_id = wave_cursor_model->materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id;
    float* saved_texcoords = wave_cursor_model->meshes[0].texcoords;
    if (wave_cursor_model->meshes[0].texcoords2 != NULL) {
        wave_cursor_model->meshes[0].texcoords = wave_cursor_model->meshes[0].texcoords2;
    }
    wave_cursor_model->meshes[0].colors = (unsigned char*)wave_cursor_colors;
    wave_cursor_model->materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = wave_cursor_texture.id;
    rlDisableDepthTest();
    rlDisableBackfaceCulling();
    DrawModelEx(*wave_cursor_model, (Vector3){0.0f, 0.0f, z_offset}, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);
    rlEnableBackfaceCulling();
    rlEnableDepthTest();
    wave_cursor_model->materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = saved_texture_id;
    wave_cursor_model->meshes[0].colors = saved_colors;
    wave_cursor_model->meshes[0].texcoords = saved_texcoords;
}

static Font font = {0};

static inline void draw_hud_text(const char* s, float x, float y, Color c) {
    for (int i = 0; s[i]; i++) {
        if (s[i] == ' ')
            continue;
        char g[2] = {s[i], 0};
        DrawTextEx(font, g, (Vector2){x + (float)i * 7.0f, y}, FONT_SIZE, 0.0f, c);
    }
}

static void draw_playback_inspection_hud(void) {
    int cursor = wave_cursor;
    int cursor_ms = SAMPLE_CURSOR_TO_MS(cursor);
    Color hud_color = is_paused ? MARINER : NEON_CARROT;

    const char* labels[] = {"CURSOR", "STEP", "SAMPLE_RATE", "TIME"};
    const char* values[] = {
        TextFormat("%6d", cursor),
        TextFormat("%4d", AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES),
        TextFormat("%5d", SRC_SAMPLE_RATE),
        TextFormat(MMSSMMM_FMT, MMSSMMM_ARGS(cursor_ms)),
    };
    float xs[] = {7.0f + 20.0f, 140.0f + 20.0f, 244.0f + 20.0f, 402.0f + 20.0f};
    for (int i = 0; i < 4; i++) {
        size_t label_len = strlen(labels[i]);
        size_t value_len = strlen(values[i]);
        DrawTextEx(font, "[", (Vector2){xs[i], 25.0f}, FONT_SIZE, 0.0f, hud_color);
        draw_hud_text(labels[i], xs[i] + 7.0f, 25.0f, hud_color);
        DrawTextEx(font, ":", (Vector2){xs[i] + (float)(label_len + 1) * 7.0f, 25.0f}, FONT_SIZE, 0.0f, hud_color);
        draw_hud_text(values[i], xs[i] + (float)(label_len + 2) * 7.0f, 25.0f, hud_color);
        DrawTextEx(font, "]", (Vector2){xs[i] + (float)(label_len + value_len + 2) * 7.0f, 25.0f}, FONT_SIZE, 0.0f, hud_color);
    }

    const char* state = is_paused ? "PAUSED" : "PLAYING";
    size_t state_len = strlen(state);
    DrawTextEx(font, "[", (Vector2){533.0f + 20.0f, 25.0f}, FONT_SIZE, 0.0f, hud_color);
    draw_hud_text(state, 533.0f + 20.0f + 7.0f, 25.0f, hud_color);
    DrawTextEx(font, "]", (Vector2){533.0f + 20.0f + (float)(state_len + 1) * 7.0f, 25.0f}, FONT_SIZE, 0.0f, hud_color);

    DrawTextEx(font, TextFormat("%2i FPS", GetFPS()), (Vector2){50.0f, 440.0f}, FONT_SIZE, 0.0f, WHITE);
    DrawTextEx(font,
               TextFormat("TRACK [%d/%d]: %s", audio_track_index, AUDIO_TRACK_COUNT - 1, AUDIO_TRACK_PATH(audio_track_index)),
               (Vector2){7.0f + 20.0f, 25.0f + FONT_SIZE},
               FONT_SIZE,
               0.0f,
               MARINER);

    if (is_paused) {
        draw_hud_text(seek_delta_chunks > 4 ? " > [...]" : "   [...]",
                      376.0f + -4.0f * 7.0f,
                      352.0f - 5.0f,
                      seek_delta_chunks > 4 ? fffftt_wave_cursor_blink_color() : MARINER);

        for (int i = 0; i < 5; i++) {
            int row_delta_chunks = seek_delta_chunks - 4 + i;
            int row_delta_ms = (row_delta_chunks * AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES * MILLISECONDS_PER_SECOND) / SRC_SAMPLE_RATE;
            int display_cursor = WRAP(paused_wave_cursor + row_delta_chunks * AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES, wave.frameCount);
            int display_cursor_ms = SAMPLE_CURSOR_TO_MS(display_cursor);
            Color row_color = (row_delta_chunks == 0) ? fffftt_wave_cursor_blink_color() : MARINER;
            float x = 376.0f + (float)(-2 + i * 2) * 7.0f;
            float y = 368.0f - 5.0f + (float)i * 16.0f;
            if (row_delta_chunks == 0)
                draw_hud_text(">", x - 14.0f, y, row_color);
            draw_hud_text(TextFormat("%6d", display_cursor), x, y, row_color);
            DrawTextEx(font, ":", (Vector2){x + 42.0f, y}, FONT_SIZE, 0.0f, row_color);
            draw_hud_text(TextFormat("%+04d", row_delta_chunks), x + 56.0f, y, row_color);
            DrawTextEx(font, "c", (Vector2){x + 84.0f, y}, FONT_SIZE, 0.0f, row_color);
            draw_hud_text(TextFormat("%+04d", row_delta_ms), x + 98.0f, y, row_color);
            DrawTextEx(font, "ms", (Vector2){x + 126.0f, y}, FONT_SIZE, 0.0f, row_color);
            draw_hud_text(TextFormat(MMSSMMM_FMT, MMSSMMM_ARGS(display_cursor_ms)), x + 147.0f, y, row_color);
        }

        draw_hud_text(seek_delta_chunks < 0 ? "  > [...]" : "    [...]",
                      376.0f + 4.0f * 7.0f,
                      448.0f - 5.0f,
                      seek_delta_chunks < 0 ? fffftt_wave_cursor_blink_color() : MARINER);
    }
}

static void update_playback_controls_sound_envelope(void) {
    if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
        reset_sticky_nav();
        if (!is_paused) {
            is_paused = true;
            wave_cursor = WRAP_MINUS(wave_cursor, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES, wave.frameCount);
            paused_wave_cursor = wave_cursor;
            seek_delta_chunks = 0;
            fffftt_audio_drain();
            PauseAudioStream(audio_stream);
            rebase_smooth_envelope_at_wave_cursor();
        } else {
            is_paused = false;
            PlayAudioStream(audio_stream);
            while (fffftt_audio_process(resume_chunk_samples)) {
                advance_lane_history(&lane_point_values[0][0], LANE_POINT_COUNT);
                smooth_lane(0);
            }
        }
    }
    if (is_paused && sticky_nav(GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
        fffftt_inspection_step_sound_envelope(BACKWARD);
    } else if (is_paused && sticky_nav(GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
        fffftt_inspection_step_sound_envelope(FORWARD);
    }
}

static void update_playback_controls_fft_spectrum(void) {
    if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
        reset_sticky_nav();

        if (!is_paused) {
            is_paused = true;
            wave_cursor = WRAP_MINUS(wave_cursor, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES, wave.frameCount);
            paused_wave_cursor = wave_cursor;
            seek_delta_chunks = 0;

            fffftt_audio_drain();

            PauseAudioStream(audio_stream);
            fffftt_inspection_fill_analysis_window(wave_cursor);
        } else {
            is_paused = false;
            PlayAudioStream(audio_stream);
        }
    }
    if (is_paused && sticky_nav(GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
        wave_cursor = WRAP_MINUS(wave_cursor, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES, wave.frameCount);
        seek_delta_chunks--;
        fffftt_inspection_fill_analysis_window(wave_cursor);
    } else if (is_paused && sticky_nav(GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
        wave_cursor = WRAP_PLUS(wave_cursor, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES, wave.frameCount);
        seek_delta_chunks++;
        fffftt_inspection_fill_analysis_window(wave_cursor);
    }
}

#define CHROMA_COUNT 12
#define CHROMA_INVERSE_LN_2 1.4426950408889634f
#define ACCIDENTALS_LUT {1.0f, 0.5f, 1.0f, 0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.5f, 1.0f, 0.5f, 1.0f}
#define ACCIDENTAL_LOOKUP(index) (((const float[CHROMA_COUNT])ACCIDENTALS_LUT)[(index)])
#define WUXING_COLOR_LOOKUP(index) (((const Color[CHROMA_COUNT])WUXING_CHROMA_LUT)[(index)])

static Color sample_chroma_palette(unsigned char chroma_id, float chroma_mask) {
    Color base_color = WUXING_COLOR_LOOKUP(chroma_id);
    float adaptive_chroma_mask_floor = adaptive_chroma_mask_mean - adaptive_chroma_mask_deviation * GLOBAL_ADAPTIVE_GATE_FLOOR_DEVIATION_SCALE;
    float adaptive_chroma_mask_ceiling = adaptive_chroma_mask_mean + adaptive_chroma_mask_deviation * GLOBAL_ADAPTIVE_GATE_CEILING_DEVIATION_SCALE;
    float gate = 0.0f;
    if (adaptive_chroma_mask_ceiling > adaptive_chroma_mask_floor) {
        gate = CLAMP((chroma_mask - adaptive_chroma_mask_floor) / (adaptive_chroma_mask_ceiling - adaptive_chroma_mask_floor), 0.0f, 1.0f);
    }
    gate = POWF(gate, 0.45f);
    float accidental_gain = 0.70f + 0.30f * ACCIDENTAL_LOOKUP(chroma_id);
    float brightness = CLAMP((0.35f + 0.65f * gate) * accidental_gain, 0.0f, 1.0f);

    return (Color){
        (unsigned char)((float)base_color.r * brightness),
        (unsigned char)((float)base_color.g * brightness),
        (unsigned char)((float)base_color.b * brightness),
        DRAW_COLOR_CHANNEL_MAX,
    };
}

static void update_mesh_colors_chroma(Color* colors, const unsigned char* lane_chroma_id, const float* chroma_mask_field, int point_count) {
    for (int i = 0; i < LANE_COUNT; i++) {
        for (int j = 0; j < point_count; j++) {
            int k = i * point_count + j;
            float chroma_mask = chroma_mask_field[k];
            Color chroma_color = sample_chroma_palette(lane_chroma_id[i], chroma_mask);
            chroma_color.a = (unsigned char)((float)DRAW_COLOR_CHANNEL_MAX * CLAMP(chroma_mask, 0.0f, 1.0f));
            colors[k] = chroma_color;
        }
    }
}

static void build_chroma_fields(unsigned char* lane_chroma_id,
                                float* chroma_mask_field,
                                const float* ridge_point_values,
                                const float* spectrum_magnitudes,
                                int point_count,
                                int chroma_bin_min,
                                int chroma_bin_max) {
    // https://librosa.org/doc/main/generated/librosa.feature.chroma_stft.html
    // https://librosa.org/doc/main/generated/librosa.filters.chroma.html
    float chroma_sum[CHROMA_COUNT] = {0};

    for (int i = chroma_bin_min; i <= chroma_bin_max; i++) {
        float frequency = ((float)i * (float)ANALYSIS_SAMPLE_RATE) / (float)ANALYSIS_WINDOW_SIZE_IN_FRAMES;
        float octave = LOGF(frequency / 27.5f) * CHROMA_INVERSE_LN_2; // 27.5 = 440.0 / 16.0, librosa hz_to_octs A0 reference
        float chroma_pos = 12.0f * octave;                            // 12.0 = n_chroma / bins_per_octave in librosa chroma default

        int next_i = MINI(i + 1, ANALYSIS_SPECTRUM_BIN_COUNT - 1);
        float next_frequency = ((float)next_i * (float)ANALYSIS_SAMPLE_RATE) / (float)ANALYSIS_WINDOW_SIZE_IN_FRAMES;
        float next_octave = LOGF(next_frequency / 27.5f) * CHROMA_INVERSE_LN_2; // 27.5 = 440.0 / 16.0, librosa hz_to_octs
        float next_chroma_pos = 12.0f * next_octave;                            // 12.0 = n_chroma
        float chroma_width = FMAXF(next_chroma_pos - chroma_pos, 1.0f);         // librosa: maximum(frqbins[1:] - frqbins[:-1], 1.0)

        float filter_l2_sum = 0.0f;
        for (int j = 0; j < CHROMA_COUNT; j++) {
            float filter_chroma = (float)j + 3.0f; // 3.0 = librosa base_c=True roll offset for 12 chroma bins
            float chroma_delta = chroma_pos - filter_chroma;
            chroma_delta = FMODF(chroma_delta + 6.0f + 120.0f, 12.0f) - 6.0f; // librosa: remainder(D + n_chroma/2 + 10*n_chroma, n_chroma) - n_chroma/2
            float chroma_filter_weight = EXPF(-0.5f * POWF((2.0f * chroma_delta) / chroma_width,
                                                           2.0f));        // librosa.filters.chroma: Gaussian bumps, exp(-0.5 * (2 * D / binwidthbins) ** 2)
            filter_l2_sum += chroma_filter_weight * chroma_filter_weight; // librosa.filters.chroma(..., norm=2): util.normalize(wts, norm=norm, axis=0)
        }

        float filter_l2_norm = SQRTF(filter_l2_sum);
        float octave_delta = (octave - 5.0f) / 2.0f;                     // 5.0 = ctroct, 2.0 = octwidth, librosa.filters.chroma defaults
        float octave_weight = EXPF(-0.5f * octave_delta * octave_delta); // librosa.filters.chroma: octave
        float magnitude = spectrum_magnitudes[i];
        float power = magnitude * magnitude; // librosa.feature.chroma_stft: _spectrogram(..., power=2)

        for (int j = 0; j < CHROMA_COUNT; j++) {
            float filter_chroma = (float)j + 3.0f; // 3.0 = librosa base_c=True roll offset for 12 chroma bins
            float chroma_delta = chroma_pos - filter_chroma;
            chroma_delta = FMODF(chroma_delta + 6.0f + 120.0f, 12.0f) - 6.0f; // librosa: project wrapped chroma distance into [-6, +6]
            float chroma_filter_weight = EXPF(-0.5f * POWF((2.0f * chroma_delta) / chroma_width, 2.0f)); // librosa Gaussian bump
            chroma_filter_weight /= filter_l2_norm;                                                      // librosa.filters.chroma(..., norm=2, axis=0)
            chroma_sum[j] += power * chroma_filter_weight * octave_weight; // librosa.feature.chroma_stft: raw_chroma = chromafb @ S
        }
    }

    float chroma_frame[CHROMA_COUNT] = {0};
    float chroma_frame_max = 0.0f;
    for (int i = 0; i < CHROMA_COUNT; i++) {
        chroma_frame_max = FMAXF(chroma_frame_max, chroma_sum[i]);
    }
    float inv_chroma_frame_max = 1.0f / chroma_frame_max; // librosa.feature.chroma_stft(..., norm=inf): util.normalize(raw_chroma, norm=norm, axis=-2)
    for (int i = 0; i < CHROMA_COUNT; i++) {
        chroma_frame[i] = CLAMP(chroma_sum[i] * inv_chroma_frame_max, 0.0f, 1.0f);
    }
    int best_chroma_id = 0;
    float best_chroma = 0.0f;
    for (int i = 0; i < CHROMA_COUNT; i++) {
        float chroma_value = chroma_frame[i];
        if (chroma_value > best_chroma) {
            best_chroma = chroma_value;
            best_chroma_id = i;
        }
    }
    *lane_chroma_id = (unsigned char)best_chroma_id;
    float chroma_mask_sum = 0.0f;
    for (int i = 0; i < point_count; i++) {
        float ridge_value = CLAMP(ridge_point_values[i], 0.0f, 1.0f);
        chroma_mask_field[i] = ridge_value * best_chroma; // custom
        chroma_mask_sum += chroma_mask_field[i];
    }
    float chroma_mask_mean = chroma_mask_sum / (float)point_count;
    if (!adaptive_chroma_mask_ready) {
        adaptive_chroma_mask_mean = chroma_mask_mean;
        adaptive_chroma_mask_deviation = chroma_mask_mean;
        adaptive_chroma_mask_ready = 1;
    } else {
        float adaptive_chroma_mask_delta = FABSF(chroma_mask_mean - adaptive_chroma_mask_mean);
        adaptive_chroma_mask_mean = LERP(adaptive_chroma_mask_mean, chroma_mask_mean, GLOBAL_ADAPTIVE_GATE_SMOOTHING_FACTOR);
        adaptive_chroma_mask_deviation = LERP(adaptive_chroma_mask_deviation, adaptive_chroma_mask_delta, GLOBAL_ADAPTIVE_GATE_SMOOTHING_FACTOR);
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
