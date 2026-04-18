#include "fffftt.h"

static const char* domain = "FFTW-DC";
static const int fftw_plan_flags = FFTW_ESTIMATE;

int main(void) {
    int16_t chunk_samples[AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES] = {0};

    FFTData fft_data = {0};
    float fft_compute_ms = 0.0f;
    fftw_plan fftw_plan_state;
    fftw_complex* fftw_input;
    fftw_complex* fftw_output;
    float analysis_window_samples[ANALYSIS_WINDOW_SIZE_IN_FRAMES] = {0};

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    float start_time = (float)GetTime();
    FFT_PROFILE_DEFINE(fft_profile_data);

    fft_data.tapback_pos = ANALYSIS_TAPBACK_POS_DEFAULT;
    fft_data.prev_spectrum_bin_levels = RL_CALLOC(ANALYSIS_SPECTRUM_BIN_COUNT, sizeof(float));
    fft_data.spectrum_history_levels = RL_CALLOC(ANALYSIS_FFT_HISTORY_FRAME_COUNT, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]));

    fftw_input = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * ANALYSIS_WINDOW_SIZE_IN_FRAMES);
    fftw_output = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * ANALYSIS_WINDOW_SIZE_IN_FRAMES);

    fftw_plan_state = fftw_create_plan_specific(ANALYSIS_WINDOW_SIZE_IN_FRAMES, FFTW_FORWARD, fftw_plan_flags, fftw_input, 1, fftw_output, 1);

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
    Wave wave = LoadWave(RD_COUNTRY_22K_WAV);
    WaveFormat(&wave, SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    AudioStream audio_stream = LoadAudioStream(SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    PlayAudioStream(audio_stream);
    size_t wave_cursor = 0;
    int16_t* wave_pcm16 = (int16_t*)wave.data;
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        while (IsAudioStreamProcessed(audio_stream)) {
            for (int i = 0; i < AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES; i++) {
                chunk_samples[i] = wave_pcm16[wave_cursor];
                if (++wave_cursor >= wave.frameCount) {
                    wave_cursor = 0;
                }
            }

            UpdateAudioStream(audio_stream, chunk_samples, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);

            for (int i = 0; i < ANALYSIS_WINDOW_SIZE_IN_FRAMES; i++) {
                analysis_window_samples[i] = (float)chunk_samples[i] / ANALYSIS_PCM16_UPPER_BOUND;
            }
        }

        apply_blackman_window_fftw_complex(fftw_input, analysis_window_samples);
        uint64_t fft_start = time_nanoseconds();
        fftw_one(fftw_plan_state, fftw_input, fftw_output);
        fft_compute_ms = elapsed_milliseconds(fft_start);

        build_spectrum_fftw(&fft_data, fftw_output);

        FFT_PROFILE_SAMPLE(fft_profile_data, domain, fft_compute_ms, (float)GetTime() - start_time, &fft_data);

        BeginDrawing();
        ClearBackground(BLACK);
        render_fft_frame(&fft_data);
        EndDrawing();
    }

    UnloadAudioStream(audio_stream);
    UnloadWave(wave);
    CloseAudioDevice();
    fftw_destroy_plan(fftw_plan_state);
    fftw_free(fftw_input);
    fftw_free(fftw_output);
    RL_FREE(fft_data.spectrum_history_levels);
    RL_FREE(fft_data.prev_spectrum_bin_levels);
    CloseWindow();
    return 0;
}
