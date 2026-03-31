#include "audio_spectrum_analyzer.h"
#include "fftw.h"
#include <stdint.h>
#include <stdlib.h>

static const char* domain = "FFTW-DC";
static const int fftw_plan_flags = FFTW_ESTIMATE;

static Wave wav = {0};
static AudioStream audio_stream = {0};
static size_t wav_cursor = 0;
static int16_t* wav_pcm16 = NULL;
static int16_t chunk_samples[AUDIO_STREAM_RING_BUFFER_SIZE] = {0};

int main(void) {
    FFTData fft_data = {0};
    float fft_compute_ms = 0.0f;
    fftw_plan fftw_plan_state;
    fftw_complex* fftw_input;
    fftw_complex* fftw_output;
    float audio_samples[FFT_WINDOW_SIZE] = {0};

    SetTraceLogLevel(LOG_WARNING); // TODO: note this should be commented out for testing logs on
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    float start_time = (float)GetTime();
    FFT_PROFILE_DEFINE(fft_profile_data);

    fft_data.tapback_pos = TAPBACK_POS_DEFAULT;
    fft_data.prev_magnitudes = RL_CALLOC(BUFFER_SIZE, sizeof(float));
    fft_data.fft_history = RL_CALLOC(FFT_HISTORY_FRAME_COUNT, sizeof(float[BUFFER_SIZE]));

    fftw_input = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * FFT_WINDOW_SIZE);
    fftw_output = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * FFT_WINDOW_SIZE);

    fftw_plan_state = fftw_create_plan_specific(FFT_WINDOW_SIZE, FFTW_FORWARD, fftw_plan_flags, fftw_input, 1, fftw_output, 1);

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_STREAM_RING_BUFFER_SIZE);
    wav = LoadWave("/rd/country_22050hz_pcm16_mono.wav");
    WaveFormat(&wav, SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    audio_stream = LoadAudioStream(SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    PlayAudioStream(audio_stream);
    wav_pcm16 = (int16_t*)wav.data;
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

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

        apply_blackman_window_fftw_complex(fftw_input, audio_samples);
        uint64_t fft_start = time_nanoseconds();
        fftw_one(fftw_plan_state, fftw_input, fftw_output);
        fft_compute_ms = elapsed_milliseconds(fft_start);

        clean_up_fftw_complex(&fft_data, fftw_output);

        FFT_PROFILE_SAMPLE(fft_profile_data, domain, fft_compute_ms, (float)GetTime() - start_time, &fft_data);

        BeginDrawing();
        ClearBackground(BLACK);
        render_frame(&fft_data);
        EndDrawing();
    }

    UnloadAudioStream(audio_stream);
    UnloadWave(wav);
    CloseAudioDevice();
    fftw_destroy_plan(fftw_plan_state);
    fftw_free(fftw_input);
    fftw_free(fftw_output);
    RL_FREE(fft_data.fft_history);
    RL_FREE(fft_data.prev_magnitudes);
    CloseWindow();
    return 0;
}
