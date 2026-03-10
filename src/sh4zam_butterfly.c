#include "audio_spectrum_analyzer.h"
#include "../sh4zam/include/sh4zam/shz_sh4zam.h"
#include <sndwav.h>
#include <dc/sound/sound.h>
#include <dc/sound/stream.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

static const char* domain = "SH4ZAM-BUTTERFLY";

static wav_stream_hnd_t wav_stream = -1;
static int16_t src_pcm_queue[DC_STREAM_QUEUE_CAPACITY] = {0};
static int src_queue_write_index = 0;
static int src_queue_read_index = 0;
static int src_queue_sample_count = 0;
static int16_t dst_pcm_ring[FFT_WINDOW_SIZE] = {0};
static int dst_ring_write_index = 0;
static int dst_ring_sample_count = 0;
static int src_sample_rate_hz = SAMPLE_RATE;
static float src_sample_fraction_carry = 0.0f;
static float fft_input_samples[FFT_WINDOW_SIZE] = {0};

static void tap_filter(wav_stream_hnd_t stream_handle,
                       void* src_sample_rate_context,
                       int src_sample_rate_hz,
                       int channel_count,
                       void** pcm_buffer,
                       int* byte_count_ptr) {
    int* src_sample_rate_out = (int*)src_sample_rate_context;
    int byte_count = *byte_count_ptr;
    int total_pcm_sample_count = byte_count / (int)sizeof(int16_t);
    int input_frame_count = total_pcm_sample_count / channel_count;
    int16_t* interleaved_pcm = (int16_t*)*pcm_buffer;

    *src_sample_rate_out = src_sample_rate_hz;

    for (int frame_index = 0; frame_index < input_frame_count; frame_index++) {
        int mixed_pcm_sum = 0;

        for (int channel_index = 0; channel_index < channel_count; channel_index++)
            mixed_pcm_sum += interleaved_pcm[frame_index * channel_count + channel_index];

        if (src_queue_sample_count >= DC_STREAM_QUEUE_CAPACITY) {
            src_queue_read_index = (src_queue_read_index + 1) % DC_STREAM_QUEUE_CAPACITY;
            src_queue_sample_count--;
        }

        src_pcm_queue[src_queue_write_index] = (int16_t)(mixed_pcm_sum / channel_count);
        src_queue_write_index = (src_queue_write_index + 1) % DC_STREAM_QUEUE_CAPACITY;
        src_queue_sample_count++;
    }
}

int main(void) {
    FFTData fft_data = {0};
    float fft_compute_ms = 0.0f;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    float start_time = (float)GetTime();
    FFT_PROFILE_DEFINE(fft_profile_data);
    fft_data.tapback_pos = TAPBACK_POS_DEFAULT;
    fft_data.work_buffer = RL_CALLOC(FFT_WINDOW_SIZE, sizeof(FFTComplex));
    fft_data.prev_magnitudes = RL_CALLOC(BUFFER_SIZE, sizeof(float));
    fft_data.fft_history = RL_CALLOC(FFT_HISTORY_FRAME_COUNT, sizeof(float[BUFFER_SIZE]));

    snd_init();
    wav_init();

    wav_stream = wav_create("/rd/country_22050hz_pcm16_mono.wav", 1);

    wav_add_filter(wav_stream, tap_filter, &src_sample_rate_hz);
    wav_volume(wav_stream, WAV_MAX_VOLUME);
    wav_play(wav_stream);
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) &&
            IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
            break;

        src_sample_fraction_carry += GetFrameTime() * (float)src_sample_rate_hz;
        int src_samples_to_copy_count = (int)floorf(src_sample_fraction_carry);
        src_sample_fraction_carry -= (float)src_samples_to_copy_count;

        while (src_samples_to_copy_count > 0 && src_queue_sample_count > 0) {
            int16_t sample_pcm = src_pcm_queue[src_queue_read_index];

            src_queue_read_index = (src_queue_read_index + 1) % DC_STREAM_QUEUE_CAPACITY;
            src_queue_sample_count--;
            dst_pcm_ring[dst_ring_write_index] = sample_pcm;
            dst_ring_write_index = (dst_ring_write_index + 1) % FFT_WINDOW_SIZE;
            if (dst_ring_sample_count < FFT_WINDOW_SIZE)
                dst_ring_sample_count++;
            src_samples_to_copy_count--;
        }

        int fft_input_available_sample_count = dst_ring_sample_count;
        int fft_input_zero_pad_count = FFT_WINDOW_SIZE - fft_input_available_sample_count;
        int dst_ring_oldest_index =
            (dst_ring_write_index - fft_input_available_sample_count + FFT_WINDOW_SIZE) % FFT_WINDOW_SIZE;

        memset(fft_input_samples, 0, (size_t)fft_input_zero_pad_count * sizeof(float));

        for (int fft_sample_offset = 0; fft_sample_offset < fft_input_available_sample_count; fft_sample_offset++) {
            int ring_index = dst_ring_oldest_index + fft_sample_offset;

            if (ring_index >= FFT_WINDOW_SIZE)
                ring_index -= FFT_WINDOW_SIZE;

            fft_input_samples[fft_input_zero_pad_count + fft_sample_offset] =
                (float)dst_pcm_ring[ring_index] / PCM_SAMPLE_MAX_F;
        }

        apply_blackman_window(&fft_data, fft_input_samples);
        float fft_start = (float)GetTime();
        shz_fft((shz_complex_t*)fft_data.work_buffer, (size_t)FFT_WINDOW_SIZE);
        fft_compute_ms = ((float)GetTime() - fft_start) * 1000.0f;

        clean_up_fft(&fft_data);

        FFT_PROFILE_SAMPLE(fft_profile_data, domain, fft_compute_ms, (float)GetTime() - start_time, &fft_data);

        BeginDrawing();
        ClearBackground(BLACK);
        render_frame(&fft_data);
        EndDrawing();
    }

    wav_stop(wav_stream);
    wav_destroy(wav_stream);
    wav_shutdown();
    snd_stream_shutdown();
    snd_shutdown();
    RL_FREE(fft_data.fft_history);
    RL_FREE(fft_data.prev_magnitudes);
    RL_FREE(fft_data.work_buffer);
    CloseWindow();
    return 0;
}
