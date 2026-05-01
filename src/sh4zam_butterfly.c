#include "fffftt.h"

static const char* domain = "SH4ZAM-BUTTERFLY";

static inline void render_fft_frame(void) {
    float frames_since_tapback = FLOORF(fft_data.tapback_pos / ((float)ANALYSIS_WINDOW_SIZE_IN_FRAMES / (float)ANALYSIS_SAMPLE_RATE));
    frames_since_tapback = CLAMP(frames_since_tapback, 0.0f, (float)(ANALYSIS_FFT_HISTORY_FRAME_COUNT - 1));
    int history_frame_index = (fft_data.history_pos - 1 - (int)frames_since_tapback + ANALYSIS_FFT_HISTORY_FRAME_COUNT) % ANALYSIS_FFT_HISTORY_FRAME_COUNT;
    float* spectrum_bin_levels = fft_data.spectrum_history_levels[history_frame_index];

    float cell_width = (float)SCREEN_WIDTH / (float)ANALYSIS_SPECTRUM_BIN_COUNT;    // fft.glsl#L19 float cellWidth = iResolution.x / NUM_OF_BINS;
    for (int bin_index = 0; bin_index < ANALYSIS_SPECTRUM_BIN_COUNT; bin_index++) { // fft.glsl#L20 float binIndex = floor(fragCoord.x / cellWidth);
        int bin_x_min = (int)FLOORF((float)bin_index * cell_width);                 //????? fft.glsl#L21 float localX = mod(fragCoord.x, cellWidth);
        int bin_x_max = (int)CEILF((float)(bin_index + 1) * cell_width);
        int bar_width = (bin_x_max - bin_x_min) - 1; // fft.glsl#L22 float barWidth = cellWidth - 1.0;

        if (bar_width < DRAW_MIN_SPECTRUM_BIN_WIDTH) {
            bar_width = DRAW_MIN_SPECTRUM_BIN_WIDTH;
        }

        float spectrum_level = spectrum_bin_levels[bin_index]; // fft.glsl#L28 float amplitude = texture(iChannel0, sampleCoord).r;
        if (spectrum_level <= 0.0f) {
            continue;
        }

        int bar_y = (int)CEILF(spectrum_level * (float)SCREEN_HEIGHT); // fft.glsl#L29 float barY = 1.0 - fragTexCoord.y;
        // fft.glsl#L30 if (barY < amplitude) color = WHITE;
        DrawRectangle(bin_x_min, SCREEN_HEIGHT - bar_y, bar_width, bar_y, WHITE);
    }
}

int main(void) {
    float fft_compute_ms = 0.0f;

    // SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    font = LoadFont(RD_FONT);
    float start_time = (float)GetTime();
    FFT_PROFILE_DEFINE(fft_profile_data);
    fft_data.tapback_pos = ANALYSIS_TAPBACK_POS_DEFAULT;
    fft_data.work_buffer = RL_CALLOC(ANALYSIS_WINDOW_SIZE_IN_FRAMES, sizeof(FFTComplex));
    fft_data.prev_spectrum_bin_levels = RL_CALLOC(ANALYSIS_SPECTRUM_BIN_COUNT, sizeof(float));
    fft_data.raw_spectrum_history_levels = RL_CALLOC(ANALYSIS_FFT_HISTORY_FRAME_COUNT, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]));
    fft_data.spectrum_history_levels = RL_CALLOC(ANALYSIS_FFT_HISTORY_FRAME_COUNT, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]));

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
    LOAD_AUDIO_TRACK(DEFAULT_AUDIO_TRACK_SHADERTOY_EXPERIMENT);
    WaveFormat(&wave, SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    audio_stream = LoadAudioStream(SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    PlayAudioStream(audio_stream);

    wave_pcm16 = (int16_t*)wave.data;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        update_playback_controls_fft_spectrum();

        while (!is_paused && IsAudioStreamProcessed(audio_stream)) {
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

        apply_blackman_window();
        uint64_t fft_start = time_nanoseconds();
        shz_fft((shz_complex_t*)fft_data.work_buffer, (size_t)ANALYSIS_WINDOW_SIZE_IN_FRAMES);
        fft_compute_ms = elapsed_milliseconds(fft_start);

        build_spectrum();

        FFT_PROFILE_SAMPLE(fft_profile_data, domain, fft_compute_ms, (float)GetTime() - start_time, &fft_data);

        update_audio_track_cycle();
        BeginDrawing();
        ClearBackground(BLACK);
        render_fft_frame();
        draw_playback_inspection_hud();
        DrawTextEx(font, TextFormat("%2i FPS", GetFPS()), (Vector2){50.0f, 440.0f}, FONT_SIZE, 0.0f, WHITE);
        EndDrawing();
    }

    UnloadAudioStream(audio_stream);
    UnloadWave(wave);
    CloseAudioDevice();
    RL_FREE(fft_data.raw_spectrum_history_levels);
    RL_FREE(fft_data.spectrum_history_levels);
    RL_FREE(fft_data.prev_spectrum_bin_levels);
    RL_FREE(fft_data.work_buffer);
    UnloadFont(font);
    CloseWindow();
    return 0;
}
