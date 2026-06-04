// https://github.com/meisei4/fffftt/blob/drafting/src/resources/fft.glsl
// https://www.shadertoy.com/view/t3jGzm
#include "fffftt.h"

static const char* domain = "SH4ZAM-BUTTERFLY";

static inline void render_fft_frame(void) {
    float frames_since_tapback = FLOORF(fft_data.tapback_pos / ((float)ANALYSIS_WINDOW_SIZE_IN_FRAMES / (float)ANALYSIS_SAMPLE_RATE));
    frames_since_tapback = CLAMP(frames_since_tapback, 0.0f, (float)(ANALYSIS_FFT_HISTORY_FRAME_COUNT - 1));
    int cur_history_frame_pos = WRAP_MINUS(fft_data.history_frame_pos, 1 + (int)frames_since_tapback, ANALYSIS_FFT_HISTORY_FRAME_COUNT);
    float* spectrum_levels = fft_data.spectrum_levels[cur_history_frame_pos];

    float cell_width = (float)SCREEN_WIDTH / (float)ANALYSIS_SPECTRUM_BIN_COUNT;    // fft.glsl#L19 float cellWidth = iResolution.x / NUM_OF_BINS;
    for (int bin_index = 0; bin_index < ANALYSIS_SPECTRUM_BIN_COUNT; bin_index++) { // fft.glsl#L20 float binIndex = floor(fragCoord.x / cellWidth);
        int bin_x_min = (int)FLOORF((float)bin_index * cell_width);                 //????? fft.glsl#L21 float localX = mod(fragCoord.x, cellWidth);
        int bin_x_max = (int)CEILF((float)(bin_index + 1) * cell_width);
        int bar_width = MAXI((bin_x_max - bin_x_min) - 1, DRAW_MIN_SPECTRUM_BIN_WIDTH); // fft.glsl#L22 float barWidth = cellWidth - 1.0;

        float spectrum_level = spectrum_levels[bin_index];                        // fft.glsl#L28 float amplitude = texture(iChannel0, sampleCoord).r;
        int bar_y = (int)CEILF(spectrum_level * (float)SCREEN_HEIGHT);            // fft.glsl#L29 float barY = 1.0 - fragTexCoord.y;
        DrawRectangle(bin_x_min, SCREEN_HEIGHT - bar_y, bar_width, bar_y, WHITE); // fft.glsl#L30 if (barY < amplitude) color = WHITE;
    }
}

int main(void) {
    float fft_compute_ms = 0.0f;

    // SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    font = LoadFont(VGA_FONT);
    float start_time = (float)GetTime();
    FFT_PROFILE_DEFINE(fft_profile_data);
    fft_data.tapback_pos = ANALYSIS_TAPBACK_POS_DEFAULT;
    fft_data.work_buffer = ALIGNED_ALLOC(ANALYSIS_WINDOW_SIZE_IN_FRAMES * sizeof(FFTComplex));
    fft_data.smoothed_spectrum_magnitudes = RL_CALLOC(ANALYSIS_SPECTRUM_BIN_COUNT, sizeof(float));
    fft_data.raw_spectrum_magnitudes = RL_CALLOC(ANALYSIS_FFT_HISTORY_FRAME_COUNT, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]));
    fft_data.spectrum_levels = RL_CALLOC(ANALYSIS_FFT_HISTORY_FRAME_COUNT, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]));

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
    set_audio_track(SHADERTOY_EXPERIMENT);
    audio_stream = LoadAudioStream(SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    PlayAudioStream(audio_stream);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        update_audio_track_cycle();
        update_playback_controls_fft_spectrum();

        while (fffftt_audio_process(chunk_samples)) {
        }

        apply_blackman_window();
        uint64_t fft_start = time_nanoseconds();
        FFT();
        fft_compute_ms = elapsed_milliseconds(fft_start);

        build_spectrum();

        FFT_PROFILE_SAMPLE(fft_profile_data, domain, fft_compute_ms, (float)GetTime() - start_time, &fft_data);
        BeginDrawing();
        ClearBackground(BLACK);
        rlDisableColorBlend();
        render_fft_frame();
        rlEnableColorBlend();
        draw_hud();
        EndDrawing();
    }

    UnloadAudioStream(audio_stream);
    unload_audio_track();
    CloseAudioDevice();
    RL_FREE(fft_data.work_buffer);
    RL_FREE(fft_data.raw_spectrum_magnitudes);
    RL_FREE(fft_data.spectrum_levels);
    RL_FREE(fft_data.smoothed_spectrum_magnitudes);
    UnloadFont(font);
    CloseWindow();
#ifdef PLATFORM_DREAMCAST
    arch_exit();
#else
    return 0;
#endif
}
