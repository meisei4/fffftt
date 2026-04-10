#include "fffftt.h"

static const char* domain = "COOL-GL33";

static Wave wav = {0};
static AudioStream audio_stream = {0};
static size_t wav_cursor = 0;
static int16_t* wav_pcm16 = NULL;
static int16_t chunk_samples[AUDIO_STREAM_RING_BUFFER_SIZE] = {0};

int main(void) {
    FFTData fft_data = {0};
    float fft_compute_ms = 0.0f;
    float audio_samples[WINDOW_SIZE] = {0};

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    float start_time = (float)GetTime();

    fft_data.tapback_pos = TAPBACK_POS_DEFAULT;
    fft_data.work_buffer = RL_CALLOC(WINDOW_SIZE, sizeof(FFTComplex));
    fft_data.prev_magnitudes = RL_CALLOC(BUFFER_SIZE, sizeof(float));
    fft_data.fft_history = RL_CALLOC(FFT_HISTORY_FRAME_COUNT, sizeof(float[BUFFER_SIZE]));

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_STREAM_RING_BUFFER_SIZE);
    wav = LoadWave(RES_COUNTRY_STEREO_44K_WAV);

    WaveFormat(&wav, SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    audio_stream = LoadAudioStream(SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);

    PlayAudioStream(audio_stream);
    wav_pcm16 = (int16_t*)wav.data;
    Image fft_image = GenImageColor(BUFFER_SIZE, 1, BLACK);
    Color* fft_pixels = (Color*)fft_image.data;
    Texture2D fft_texture = LoadTextureFromImage(fft_image);
    Shader shader = LoadShader(NULL, SHADER_FFT);
    int resolution_location = GetShaderLocation(shader, "iResolution");
    int channel_location = GetShaderLocation(shader, "iChannel0");
    Vector2 resolution = {(float)SCREEN_WIDTH, (float)SCREEN_HEIGHT};

    SetTextureFilter(fft_texture, TEXTURE_FILTER_POINT);
    SetShaderValue(shader, resolution_location, &resolution, SHADER_UNIFORM_VEC2);
    SetShaderValueTexture(shader, channel_location, fft_texture);
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

            for (int i = 0; i < WINDOW_SIZE; i++) {
                audio_samples[i] = (float)chunk_samples[WINDOW_SIZE + i] / PCM_SAMPLE_MAX_F;
            }
        }

        apply_blackman_window(&fft_data, audio_samples);
        float fft_start = (float)GetTime();
        cooley_tukey_fft_slow(fft_data.work_buffer);
        fft_compute_ms = ((float)GetTime() - fft_start) * 1000.0f;

        clean_up_fft(&fft_data);

        FFT_PROFILE_SAMPLE(fft_profile_data, domain, fft_compute_ms, (float)GetTime() - start_time, &fft_data);

        float frames_since_tapback = floorf(fft_data.tapback_pos / ((float)WINDOW_SIZE / EFFECTIVE_SAMPLE_RATE));
        frames_since_tapback = fminf(fmaxf(frames_since_tapback, 0.0f), (float)(FFT_HISTORY_FRAME_COUNT - 1));
        int history_position = (fft_data.history_pos - 1 - (int)frames_since_tapback + FFT_HISTORY_FRAME_COUNT) % FFT_HISTORY_FRAME_COUNT;

        for (int bin = 0; bin < BUFFER_SIZE; bin++) {
            float amplitude = fft_data.fft_history[history_position][bin];
            float normalized_amplitude = fminf(fmaxf(roundf(amplitude * (float)COLOR_CHANNEL_MAX), 0.0f), (float)COLOR_CHANNEL_MAX);
            fft_pixels[bin] = (Color){(unsigned char)normalized_amplitude, 0, 0, COLOR_CHANNEL_MAX};
        }
        UpdateTexture(fft_texture, fft_image.data);

        BeginDrawing();
        ClearBackground(BLACK);
        BeginShaderMode(shader);
        SetShaderValueTexture(shader, channel_location, fft_texture);
        DrawTexturePro(fft_texture,
                       (Rectangle){0.0f, 0.0f, (float)fft_texture.width, (float)fft_texture.height},
                       (Rectangle){0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT},
                       (Vector2){0},
                       0.0f,
                       WHITE);
        EndShaderMode();
        EndDrawing();
    }

    UnloadShader(shader);
    UnloadTexture(fft_texture);
    UnloadImage(fft_image);
    UnloadAudioStream(audio_stream);
    UnloadWave(wav);
    CloseAudioDevice();
    RL_FREE(fft_data.fft_history);
    RL_FREE(fft_data.prev_magnitudes);
    RL_FREE(fft_data.work_buffer);
    CloseWindow();
    return 0;
}
