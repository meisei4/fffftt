#include "audio_spectrum_analyzer.h"
#include <stdint.h>

#include "../fftw_1997/fftw.h"

static const char* domain = "FFTW-GL11";

static Wave wav = {0};
static AudioStream audio_stream = {0};
static size_t wav_cursor = 0;
static int16_t* wav_pcm16 = NULL;
static int16_t chunk_samples[AUDIO_STREAM_RING_BUFFER_SIZE] = {0};

int main(void) {
    FFTData fft_data = {0};
    float fft_compute_ms = 0.0f;
    float audio_samples[FFT_WINDOW_SIZE] = {0};

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    float start_time = (float)GetTime();

    fft_data.tapback_pos = TAPBACK_POS_DEFAULT;
    fft_data.work_buffer = RL_CALLOC(FFT_WINDOW_SIZE, sizeof(FFTComplex));
    fft_data.prev_magnitudes = RL_CALLOC(BUFFER_SIZE, sizeof(float));
    fft_data.fft_history = RL_CALLOC(FFT_HISTORY_FRAME_COUNT, sizeof(float[BUFFER_SIZE]));

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_STREAM_RING_BUFFER_SIZE);
    wav = LoadWave("src/resources/country_44100hz_pcm16_stereo.wav");

    WaveFormat(&wav, SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    audio_stream = LoadAudioStream(SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);

    PlayAudioStream(audio_stream);
    wav_pcm16 = (int16_t*)wav.data;
    fftw_complex* fftw_input = fftw_malloc(sizeof(fftw_complex) * FFT_WINDOW_SIZE);
    fftw_complex* fftw_output = fftw_malloc(sizeof(fftw_complex) * FFT_WINDOW_SIZE);
    fftw_plan fftw_plan_state = fftw_create_plan(FFT_WINDOW_SIZE, FFTW_FORWARD, FFTW_ESTIMATE);
    SetTargetFPS(60);

    FFT_PROFILE_DEFINE(fft_profile_data);

    while (!WindowShouldClose()) {
        while (IsAudioStreamProcessed(audio_stream)) {
            for (int i = 0; i < AUDIO_STREAM_RING_BUFFER_SIZE; i++) {
                chunk_samples[i] = wav_pcm16[wav_cursor];
                if (++wav_cursor >= wav.frameCount) {
                    wav_cursor = 0;
                }
            }

            UpdateAudioStream(audio_stream, chunk_samples, AUDIO_STREAM_RING_BUFFER_SIZE);

            for (int i = 0; i < FFT_WINDOW_SIZE; i++) {
                audio_samples[i] = (float)chunk_samples[FFT_WINDOW_SIZE + i] / PCM_SAMPLE_MAX_F;
            }
        }

        apply_blackman_window(&fft_data, audio_samples);
        float fft_start = (float)GetTime();
        for (int i = 0; i < FFT_WINDOW_SIZE; i++) {
            fftw_input[i].re = (fftw_real)fft_data.work_buffer[i].real;
            fftw_input[i].im = (fftw_real)fft_data.work_buffer[i].imaginary;
        }
        fftw_one(fftw_plan_state, fftw_input, fftw_output);
        for (int i = 0; i < FFT_WINDOW_SIZE; i++) {
            fft_data.work_buffer[i].real = (float)fftw_output[i].re;
            fft_data.work_buffer[i].imaginary = (float)fftw_output[i].im;
        }
        fft_compute_ms = ((float)GetTime() - fft_start) * 1000.0f;

        clean_up_fft(&fft_data);

        FFT_PROFILE_SAMPLE(fft_profile_data, domain, fft_compute_ms, (float)GetTime() - start_time, &fft_data);

        BeginDrawing();
        ClearBackground(BLACK);
        render_frame(&fft_data);
        EndDrawing();
    }

    fftw_destroy_plan(fftw_plan_state);
    fftw_free(fftw_input);
    fftw_free(fftw_output);
    UnloadAudioStream(audio_stream);
    UnloadWave(wav);
    CloseAudioDevice();
    RL_FREE(fft_data.fft_history);
    RL_FREE(fft_data.prev_magnitudes);
    RL_FREE(fft_data.work_buffer);
    CloseWindow();
    return 0;
}
