#include "audio_spectrum_analyzer.h"
#include "raylib.h"
#include <math.h>
#include <stdint.h>

static const char* domain = "SOUND-ENVELOPE-GL33";

#define LANE_COUNT 5
#define LANE_POINT_COUNT 64
#define AUDIO_TEXTURE_ROW_COUNT 2

static float waveform_window_samples[WAVEFORM_WINDOW_SIZE] = {0};

static void update_audio_texture_pixels(Color* audio_texture_pixels);

int main(void) {
    int16_t chunk_samples[WAVEFORM_AUDIO_STREAM_RING_BUFFER_SIZE] = {0};

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(WAVEFORM_AUDIO_STREAM_RING_BUFFER_SIZE);
    Wave wave = LoadWave("src/resources/shadertoy_experiment_22050hz_pcm16_mono.wav");
    WaveFormat(&wave, SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    AudioStream stream = LoadAudioStream(SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    PlayAudioStream(stream);

    size_t wave_cursor = 0;
    int16_t* pcm_data = (int16_t*)wave.data;

    Image audio_texture_image = GenImageColor(WAVEFORM_WINDOW_SIZE, AUDIO_TEXTURE_ROW_COUNT, BLACK);
    Color* audio_texture_pixels = (Color*)audio_texture_image.data;
    Texture2D audio_texture = LoadTextureFromImage(audio_texture_image);

    Shader buffer_a = LoadShader(0, "src/resources/sound_envelope_buffer_a.glsl");
    Shader image = LoadShader(0, "src/resources/sound_envelope_image.glsl");

    int buffer_iresolution_loc = GetShaderLocation(buffer_a, "iResolution");
    int buffer_iframe_loc = GetShaderLocation(buffer_a, "iFrame");
    int buffer_ichannel0_loc = GetShaderLocation(buffer_a, "iChannel0");
    int buffer_ichannel1_loc = GetShaderLocation(buffer_a, "iChannel1");

    int image_iresolution_loc = GetShaderLocation(image, "iResolution");
    int image_ichannel0_loc = GetShaderLocation(image, "iChannel0");

    RenderTexture2D feed = LoadRenderTexture(LANE_POINT_COUNT, LANE_COUNT);
    RenderTexture2D back = LoadRenderTexture(LANE_POINT_COUNT, LANE_COUNT);
    RenderTexture2D* prev_render_texture = &feed;
    RenderTexture2D* cur_render_texture = &back;

    int frame = 0;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        while (IsAudioStreamProcessed(stream)) {
            for (int i = 0; i < WAVEFORM_AUDIO_STREAM_RING_BUFFER_SIZE; i++) {
                chunk_samples[i] = pcm_data[wave_cursor];
                if (++wave_cursor >= wave.frameCount) {
                    wave_cursor = 0;
                }
            }

            UpdateAudioStream(stream, chunk_samples, WAVEFORM_AUDIO_STREAM_RING_BUFFER_SIZE);

            for (int i = 0; i < WAVEFORM_WINDOW_SIZE; i++) {
                waveform_window_samples[i] = (float)chunk_samples[WAVEFORM_WINDOW_SIZE + i] / PCM_SAMPLE_MAX_F;
            }
        }

        update_audio_texture_pixels(audio_texture_pixels);
        UpdateTexture(audio_texture, audio_texture_pixels);

        float buffer_resolution[3] = {(float)LANE_POINT_COUNT, (float)LANE_COUNT, 1.0f};
        float image_resolution[3] = {(float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 1.0f};

        BeginTextureMode(*cur_render_texture);
        ClearBackground(BLACK);
        BeginShaderMode(buffer_a);
        SetShaderValue(buffer_a, buffer_iresolution_loc, buffer_resolution, SHADER_UNIFORM_VEC3);
        SetShaderValue(buffer_a, buffer_iframe_loc, &frame, SHADER_UNIFORM_INT);
        SetShaderValueTexture(buffer_a, buffer_ichannel0_loc, prev_render_texture->texture);
        SetShaderValueTexture(buffer_a, buffer_ichannel1_loc, audio_texture);
        DrawRectangle(0, 0, LANE_POINT_COUNT, LANE_COUNT, WHITE);
        EndShaderMode();
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        BeginShaderMode(image);
        SetShaderValue(image, image_iresolution_loc, image_resolution, SHADER_UNIFORM_VEC3);
        SetShaderValueTexture(image, image_ichannel0_loc, cur_render_texture->texture);
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
        EndShaderMode();
        EndDrawing();

        RenderTexture2D* temp_render_texture = prev_render_texture;
        prev_render_texture = cur_render_texture;
        cur_render_texture = temp_render_texture;
        frame++;
    }

    UnloadRenderTexture(feed);
    UnloadRenderTexture(back);
    UnloadShader(image);
    UnloadShader(buffer_a);
    UnloadTexture(audio_texture);
    UnloadImage(audio_texture_image);
    UnloadAudioStream(stream);
    UnloadWave(wave);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

static void update_audio_texture_pixels(Color* audio_texture_pixels) {
    for (int i = 0; i < WAVEFORM_WINDOW_SIZE; i++) {
        float amplitude = fabsf(waveform_window_samples[i]);
        unsigned char pixel_value = (unsigned char)(fminf(fmaxf(amplitude, 0.0f), 1.0f) * 255.0f);
        audio_texture_pixels[i].r = pixel_value;
        audio_texture_pixels[i].g = pixel_value;
        audio_texture_pixels[i].b = pixel_value;
        audio_texture_pixels[i].a = 255;
        audio_texture_pixels[i + WAVEFORM_WINDOW_SIZE].r = pixel_value;
        audio_texture_pixels[i + WAVEFORM_WINDOW_SIZE].g = pixel_value;
        audio_texture_pixels[i + WAVEFORM_WINDOW_SIZE].b = pixel_value;
        audio_texture_pixels[i + WAVEFORM_WINDOW_SIZE].a = 255;
    }
}
