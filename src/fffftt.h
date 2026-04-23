#ifndef FFFFTT_H
#define FFFFTT_H

#include "raylib.h"
#include "raymath.h"
#include "../fftw_1997/fftw.h"
#include "../sh4zam/include/sh4zam/shz_sh4zam.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
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
#define MEMCPY(dst, src, size) memcpy((dst), (src), (size)) //TODO: Colors... hmmm
#define MEMCPY4(dst, src, size) shz_memcpy4((dst), (src), (size))
#define CLAMP(x, min, max) shz_clampf((x), (min), (max))
#define LERP(a, b, t) shz_lerpf((a), (b), (t))

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

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

#define AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES 2048

#define Y_AXIS (Vector3){0.0f, 1.0f, 0.0f}
#define DEFAULT_SCALE (Vector3){1.0f, 1.0f, 1.0f}

#define CAMERA_FOVY_MIN 0.1f
#define CAMERA_FOVY_MAX 10.0f
#define CAMERA_FOVY_VELOCITY 6.0f
#define CAMERA_ORBIT_VELOCITY 2.0f
#define CAMERA_PITCH_MIN (-PI/2.0f + 0.1f)
#define CAMERA_PITCH_MAX ( PI/2.0f - 0.1f)


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

#define RD_DDS_FFM_22K_WAV "/rd/dds_ffm_22050hz_pcm16_mono.wav"

#define SHADER_FFT "src/resources/fft.glsl"
#define SHADER_WAVEFORM "src/resources/waveform.glsl"
#define SHADER_SOUND_ENVELOPE_BUFFER_A "src/resources/sound_envelope_buffer_a.glsl"
#define SHADER_SOUND_ENVELOPE_IMAGE "src/resources/sound_envelope_image.glsl"

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

#define DRAW_COLOR_CHANNEL_MAX 255
#define DRAW_MIN_SPECTRUM_BIN_WIDTH 1

typedef struct FFTComplex {
    float real;
    float imaginary;
} FFTComplex;

typedef struct FFTData {
    FFTComplex *work_buffer;
    float *prev_spectrum_bin_levels;
    unsigned int frame_index;
    float (*spectrum_history_levels)[ANALYSIS_SPECTRUM_BIN_COUNT];
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

static inline void apply_blackman_window(FFTData* fft_data, float* analysis_window_samples) {
    for (int i = 0; i < ANALYSIS_WINDOW_SIZE_IN_FRAMES; i++) {
        float x = (2.0f * PI * i) / (ANALYSIS_WINDOW_SIZE_IN_FRAMES - 1.0f);
        float blackman_weight = ANALYSIS_BLACKMAN_A - ANALYSIS_BLACKMAN_B * cosf(x) + ANALYSIS_BLACKMAN_C * cosf(2.0f * x);
        float sample = analysis_window_samples[i];

        fft_data->work_buffer[i].real = sample * blackman_weight;
        fft_data->work_buffer[i].imaginary = 0.0f;
    }
}

static inline void apply_blackman_window_fftw_complex(fftw_complex* fft_input, float* analysis_window_samples) {
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

static inline void update_spectrum_bin(FFTData* fft_data, float* smoothed_spectrum, int bin, float real, float imaginary) {
    float linear_magnitude = sqrtf(real * real + imaginary * imaginary) / ANALYSIS_WINDOW_SIZE_IN_FRAMES;
    float smoothed_magnitude =
        ANALYSIS_SMOOTHING_TIME_CONSTANT * fft_data->prev_spectrum_bin_levels[bin] + (1.0f - ANALYSIS_SMOOTHING_TIME_CONSTANT) * linear_magnitude;
    float db = logf(fmaxf(smoothed_magnitude, ANALYSIS_MIN_LOG_MAGNITUDE)) * ANALYSIS_DB_TO_LINEAR_SCALE;
    float normalized = (db - ANALYSIS_MIN_DECIBELS) * ANALYSIS_INVERSE_DECIBEL_RANGE;
    float clamped_magnitude = Clamp(normalized, 0.0f, 1.0f);

    fft_data->prev_spectrum_bin_levels[bin] = smoothed_magnitude;
    smoothed_spectrum[bin] = clamped_magnitude;
}

static inline void build_spectrum(FFTData* fft_data) {
    float* smoothed_spectrum = fft_data->spectrum_history_levels[fft_data->history_pos];

    for (int bin = 0; bin < ANALYSIS_SPECTRUM_BIN_COUNT; bin++) {
        float re = fft_data->work_buffer[bin].real;
        float im = fft_data->work_buffer[bin].imaginary;
        update_spectrum_bin(fft_data, smoothed_spectrum, bin, re, im);
    }

    fft_data->history_pos = (fft_data->history_pos + 1) % ANALYSIS_FFT_HISTORY_FRAME_COUNT;
    fft_data->frame_index++;
}

static inline void build_spectrum_fftw(FFTData* fft_data, fftw_complex* fft_output) {
    float* smoothed_spectrum = fft_data->spectrum_history_levels[fft_data->history_pos];

    for (int bin = 0; bin < ANALYSIS_SPECTRUM_BIN_COUNT; bin++) {
        float re = (float)fft_output[bin].re;
        float im = (float)fft_output[bin].im;
        update_spectrum_bin(fft_data, smoothed_spectrum, bin, re, im);
    }

    fft_data->history_pos = (fft_data->history_pos + 1) % ANALYSIS_FFT_HISTORY_FRAME_COUNT;
    fft_data->frame_index++;
}

static inline void render_fft_frame(FFTData* fft_data) {
    float frames_since_tapback = floorf(fft_data->tapback_pos / ((float)ANALYSIS_WINDOW_SIZE_IN_FRAMES / (float)ANALYSIS_SAMPLE_RATE));
    frames_since_tapback = Clamp(frames_since_tapback, 0.0f, (float)(ANALYSIS_FFT_HISTORY_FRAME_COUNT - 1));
    int history_frame_index = (fft_data->history_pos - 1 - (int)frames_since_tapback + ANALYSIS_FFT_HISTORY_FRAME_COUNT) % ANALYSIS_FFT_HISTORY_FRAME_COUNT;
    float* spectrum_bin_levels = fft_data->spectrum_history_levels[history_frame_index];

    float cell_width = (float)SCREEN_WIDTH / (float)ANALYSIS_SPECTRUM_BIN_COUNT; // fft.glsl#L19 float cellWidth = iResolution.x / NUM_OF_BINS;
    for (int bin_index = 0; bin_index < ANALYSIS_SPECTRUM_BIN_COUNT; bin_index++) { // fft.glsl#L20 float binIndex = floor(fragCoord.x / cellWidth);
        int bin_x_min = (int)floorf((float)bin_index * cell_width); //????? fft.glsl#L21 float localX = mod(fragCoord.x, cellWidth);
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
    #define AMPLITUDE_Y_SCALE 2.0f
    // #define AMPLITUDE_Y_SCALE 0.66f
    #define LINE_LENGTH_SCALE 5.0f
#else //FFFFTT_PROFILE_FFT_TERRAIN_3D...
    #define LANE_COUNT 11
    #define LANE_POINT_COUNT 33
    #define AMPLITUDE_Y_SCALE 3.0f
    #define LINE_LENGTH_SCALE 5.0f
#endif

#define TOP (Vector3){0.0f, 2.0f, 0.0f}
#define MIDDLE (Vector3){0.0f}
#define BOTTOM (Vector3){0.0f, -1.0f, 0.0f}

#define HALF_SPAN 0.5f
#define ISOMETRIC_ZOOM 3.0f
#define ISOMETRIC_GRID_CENTER_X (0.5f * (-(float)(LANE_COUNT - 1) * ISOMETRIC_LANE_SPACING + (float)(LANE_POINT_COUNT - 1)))
#define ISOMETRIC_GRID_CENTER_Y (0.25f * ((float)(LANE_POINT_COUNT - 1) + (float)(LANE_COUNT - 1) * ISOMETRIC_LANE_SPACING))
#define FRONT_LANE_SMOOTHING 0.4f
#define ISOMETRIC_LANE_SPACING 9.0f
#define POINT_SIZE_RASTER_PIXELS 3.0f
#define LANE_SPACING_SCALE 0.50f
#define MESH_VERTEX_COUNT (LANE_COUNT * LANE_POINT_COUNT)
#define LINE_SEGMENT_COUNT (LANE_COUNT * (LANE_POINT_COUNT - 1))
#define LINE_INDEX_COUNT (LINE_SEGMENT_COUNT * 2)
#define WAVEFORM_SAMPLES_PER_LANE_POINT (ANALYSIS_WINDOW_SIZE_IN_FRAMES / LANE_POINT_COUNT)

static inline void advance_lane_history(float* lane_point_values) {
    for (int i = LANE_COUNT - 1; i > 0; i--) {
        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            lane_point_values[i * LANE_POINT_COUNT + j] = lane_point_values[(i - 1) * LANE_POINT_COUNT + j];
        }
    }
}

static inline void smooth_front_lane(float* lane_point_values, float* analysis_window_samples) {
    for (int i = 0; i < LANE_POINT_COUNT; i++) {
        float sample_sum = 0.0f;
        int k = i * WAVEFORM_SAMPLES_PER_LANE_POINT;
        for (int j = 0; j < WAVEFORM_SAMPLES_PER_LANE_POINT; j++) {
            sample_sum += fabsf(analysis_window_samples[k + j]);
        }

        lane_point_values[i] = (sample_sum / (float)WAVEFORM_SAMPLES_PER_LANE_POINT) * FRONT_LANE_SMOOTHING;
    }
}

static inline void update_envelope_mesh_vertices_isometric(Vector3* vertices, float* lane_point_values) {
    for (int i = 0; i < LANE_COUNT; i++) {
        float lane_offset = (float)i * ISOMETRIC_LANE_SPACING;
        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            int k = i * LANE_POINT_COUNT + j;
            float grid_x = (float)j - lane_offset;
            float grid_y = 0.5f * ((float)j + lane_offset) - lane_point_values[k] * AMPLITUDE_Y_SCALE;
            vertices[k].x = 0.5f * (float)SCREEN_WIDTH + (grid_x - ISOMETRIC_GRID_CENTER_X) * ISOMETRIC_ZOOM;
            vertices[k].y = 0.5f * (float)SCREEN_HEIGHT - (grid_y - ISOMETRIC_GRID_CENTER_Y) * ISOMETRIC_ZOOM;
            vertices[k].z = 0.0f;
        }
    }
}

static inline void update_envelope_mesh_vertices(Vector3* vertices, float* lane_point_values) {
    for (int i = 0; i < LANE_COUNT; i++) {
        float z = HALF_SPAN - ((float)i / (float)(LANE_COUNT - 1));
        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            int k = i * LANE_POINT_COUNT + j;
            float x = (((float)j / (float)(LANE_POINT_COUNT - 1)) - HALF_SPAN) * LINE_LENGTH_SCALE;
            vertices[k].x = x;
            vertices[k].y = lane_point_values[k] * AMPLITUDE_Y_SCALE;
            vertices[k].z = z;
        }
    }
}

static inline void update_mesh_vertices(float* vertices, float* lane_point_values) {
    for (int i = 0; i < LANE_COUNT; i++) {
        float lane_offset = ((float)i - 0.5f * (float)(LANE_COUNT - 1)) * LANE_SPACING_SCALE;
        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            int k = (i * LANE_POINT_COUNT + j) * 3;
            float x = (((float)j / (float)(LANE_POINT_COUNT - 1)) - HALF_SPAN) * LINE_LENGTH_SCALE;
            float y = lane_point_values[i * LANE_POINT_COUNT + j] * AMPLITUDE_Y_SCALE;
            vertices[k + 0] = x;
            vertices[k + 1] = y;
            vertices[k + 2] = lane_offset; // NOTE: +Z orientation determined by raylib GenMeshPlane function!!
        }
    }
}

static inline void fill_mesh_colors(Color* colors) {
    for (int i = 0; i < LANE_COUNT; i++) {
        float v = (float)i / (float)(LANE_COUNT - 1);
        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            float u = (float)j / (float)(LANE_POINT_COUNT - 1);
            int k = i * LANE_POINT_COUNT + j;
            float r = (1.0f - u) * (1.0f - v) * MAGENTA.r + u * (1.0f - v) * BLUE.r + (1.0f - u) * v * RED.r + u * v * YELLOW.r;
            float g = (1.0f - u) * (1.0f - v) * MAGENTA.g + u * (1.0f - v) * BLUE.g + (1.0f - u) * v * RED.g + u * v * YELLOW.g;
            float b = (1.0f - u) * (1.0f - v) * MAGENTA.b + u * (1.0f - v) * BLUE.b + (1.0f - u) * v * RED.b + u * v * YELLOW.b;
            colors[k] = (Color){(unsigned char)r, (unsigned char)g, (unsigned char)b, DRAW_COLOR_CHANNEL_MAX};
        }
    }
}

#define TERRAIN_TRIANGLE_COUNT (((LANE_COUNT - 1) * (LANE_POINT_COUNT - 1)) * 2)
#define FLAT_VERTEX_COUNT (TERRAIN_TRIANGLE_COUNT * 3)

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

static void update_mesh_normals_smooth(float* normals, const float* vertices) {
    for (int i = 0; i < LANE_COUNT; i++) {
        int i_prev = (i > 0) ? i - 1 : i;
        int i_next = (i < LANE_COUNT - 1) ? i + 1 : i;

        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            int j_prev = (j > 0) ? j - 1 : j;
            int j_next = (j < LANE_POINT_COUNT - 1) ? j + 1 : j;

            int k_left = (i * LANE_POINT_COUNT + j_prev) * 3;
            int k_right = (i * LANE_POINT_COUNT + j_next) * 3;
            int k_back = (i_prev * LANE_POINT_COUNT + j) * 3;
            int k_front = (i_next * LANE_POINT_COUNT + j) * 3;
            int k_out = (i * LANE_POINT_COUNT + j) * 3;

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

static void build_mesh_smooth(Mesh* dst_mesh, const float* vertices, const float* src_normals, const Color* src_colors) {
    MEMCPY4(dst_mesh->vertices, vertices, sizeof(float) * MESH_VERTEX_COUNT * 3);
    MEMCPY4(dst_mesh->normals, src_normals, sizeof(float) * MESH_VERTEX_COUNT * 3);
    MEMCPY(dst_mesh->colors, src_colors, sizeof(Color) * MESH_VERTEX_COUNT);
}

static void build_mesh_flat(Mesh* dst_mesh, const float* flat_vertices, const float* src_normals, const Color* src_colors) {
    MEMCPY4(dst_mesh->vertices, flat_vertices, sizeof(float) * FLAT_VERTEX_COUNT * 3);
    MEMCPY4(dst_mesh->normals, src_normals, sizeof(float) * FLAT_VERTEX_COUNT * 3);
    MEMCPY(dst_mesh->colors, src_colors, sizeof(Color) * FLAT_VERTEX_COUNT);
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

#define LANE_MASK_TEXTURE_WIDTH 8
#define LANE_MASK_TEXTURE_HEIGHT 8
#define LANE_MASK_STRIPE_OFFSET 4
#define LANE_MASK_WIDTH_TEXELS 1
#define LANE_MASK_TEXCOORD_S_CENTER (0.5f / (float)LANE_MASK_TEXTURE_WIDTH)
#define LANE_MASK_TEXCOORD_FRONT_LANE_INNER_EDGE ((float)LANE_MASK_STRIPE_OFFSET / (float)LANE_MASK_TEXTURE_HEIGHT)
#define LANE_MASK_TEXCOORD_MID_LANE_CENTER (((float)LANE_MASK_STRIPE_OFFSET + 0.5f) / (float)LANE_MASK_TEXTURE_HEIGHT)
#define LANE_MASK_TEXCOORD_BACK_LANE_INNER_EDGE (((float)LANE_MASK_STRIPE_OFFSET + (float)LANE_MASK_WIDTH_TEXELS) / (float)LANE_MASK_TEXTURE_HEIGHT)

static void fill_lane_mask_texcoords(float* texcoords) {
    float s = LANE_MASK_TEXCOORD_S_CENTER;
    for (int i = 0; i < LANE_COUNT; i++) {
        float t = (float)i + LANE_MASK_TEXCOORD_MID_LANE_CENTER;
        if (i == 0) {
            t = LANE_MASK_TEXCOORD_FRONT_LANE_INNER_EDGE;
        } else if (i == LANE_COUNT - 1) {
            t = (float)i + LANE_MASK_TEXCOORD_BACK_LANE_INNER_EDGE;
        }
        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            int k = i * LANE_POINT_COUNT + j;
            texcoords[k * 2 + 0] = s;
            texcoords[k * 2 + 1] = t;
        }
    }
}

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

static Texture2D build_lane_mask(float* texcoords) {
    Image image = {
        .data = RL_CALLOC(LANE_MASK_TEXTURE_WIDTH * LANE_MASK_TEXTURE_HEIGHT, sizeof(unsigned short)),
        .width = LANE_MASK_TEXTURE_WIDTH,
        .height = LANE_MASK_TEXTURE_HEIGHT,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R4G4B4A4,
    };
    unsigned short* pixels = (unsigned short*)image.data;
    fill_lane_mask_texcoords(texcoords);
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

static Texture2D build_lane_mask_glow(float* texcoords) {
    Image image = {
        .data = RL_CALLOC(LANE_MASK_TEXTURE_WIDTH * LANE_MASK_TEXTURE_HEIGHT, sizeof(unsigned short)),
        .width = LANE_MASK_TEXTURE_WIDTH,
        .height = LANE_MASK_TEXTURE_HEIGHT,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R4G4B4A4,
    };
    unsigned short* pixels = (unsigned short*)image.data;
    fill_lane_mask_texcoords(texcoords);
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

#endif // FFFFTT_H
