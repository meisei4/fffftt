#include "audio_spectrum_analyzer.h"
#include "raylib.h"
#include <math.h>
#include <stdint.h>

static const char* domain = "SOUND-ENVELOPE-GL33";

#define LANE_COUNT 5
#define LANE_POINT_COUNT 64
#define WAVEFORM_SAMPLES_PER_LANE_POINT (WAVEFORM_BUFFER_SIZE / LANE_POINT_COUNT)

#define AMPLITUDE_Y_SCALE 120.0f
#define LANE_SPACING 9.0f
#define ISOMETRIC_ZOOM 3.0f
#define ISOMETRIC_GRID_CENTER_X (0.5f * (-(float)(LANE_COUNT - 1) * LANE_SPACING + (float)(LANE_POINT_COUNT - 1)))
#define ISOMETRIC_GRID_CENTER_Y (0.25f * ((float)(LANE_POINT_COUNT - 1) + (float)(LANE_COUNT - 1) * LANE_SPACING))
#define FRONT_LANE_SMOOTHING 0.4f

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

    Image audio_texture_image = GenImageColor(WAVEFORM_BUFFER_SIZE, AUDIO_TEXTURE_ROW_COUNT, BLACK);
    Color* audio_texture_pixels = (Color*)audio_texture_image.data;
    Texture2D audio_texture = LoadTextureFromImage(audio_texture_image);

    Shader buffer_a = LoadShader(0, "src/resources/sound_envelope_buffer_a.glsl");
    Shader image = LoadShader(0, "src/resources/sound_envelope_image.glsl");

    int buffer_iresolution_loc = GetShaderLocation(buffer_a, "iResolution");
    int buffer_ichannel0_loc = GetShaderLocation(buffer_a, "iChannel0");
    int buffer_ichannel1_loc = GetShaderLocation(buffer_a, "iChannel1");
    int buffer_uadvancehistory_loc = GetShaderLocation(buffer_a, "uAdvanceHistory");
    int buffer_ufrontlanesmoothing_loc = GetShaderLocation(buffer_a, "uFrontLaneSmoothing");

    int image_iresolution_loc = GetShaderLocation(image, "iResolution");
    int image_ichannel0_loc = GetShaderLocation(image, "iChannel0");
    int image_uamplitudescale_loc = GetShaderLocation(image, "u_amplitude_scale");
    int image_urowspacing_loc = GetShaderLocation(image, "u_row_spacing");
    int image_uisometriczoom_loc = GetShaderLocation(image, "u_isometric_zoom");
    int image_ugridcenter_loc = GetShaderLocation(image, "u_grid_center");
    int image_ulinerenderwidth_loc = GetShaderLocation(image, "u_line_render_width");

    RenderTexture2D feed = LoadRenderTexture(LANE_POINT_COUNT, LANE_COUNT);
    RenderTexture2D back = LoadRenderTexture(LANE_POINT_COUNT, LANE_COUNT);
    RenderTexture2D* prev_render_texture = &feed;
    RenderTexture2D* cur_render_texture = &back;
    float image_grid_center[2] = {
        ISOMETRIC_GRID_CENTER_X,
        ISOMETRIC_GRID_CENTER_Y,
    };
    const float image_line_render_width = 0.75f;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        bool audio_window_advanced = false;

        while (IsAudioStreamProcessed(stream)) {
            audio_window_advanced = true;
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
        int advance_history = audio_window_advanced ? 1 : 0;

        BeginTextureMode(*cur_render_texture);
        ClearBackground(BLACK);
        BeginShaderMode(buffer_a);
        SetShaderValue(buffer_a, buffer_iresolution_loc, buffer_resolution, SHADER_UNIFORM_VEC3);
        SetShaderValueTexture(buffer_a, buffer_ichannel0_loc, prev_render_texture->texture);
        SetShaderValueTexture(buffer_a, buffer_ichannel1_loc, audio_texture);
        SetShaderValue(buffer_a, buffer_uadvancehistory_loc, &advance_history, SHADER_UNIFORM_INT);
        SetShaderValue(buffer_a, buffer_ufrontlanesmoothing_loc, &(float){FRONT_LANE_SMOOTHING}, SHADER_UNIFORM_FLOAT);
        DrawRectangle(0, 0, LANE_POINT_COUNT, LANE_COUNT, WHITE);
        EndShaderMode();
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        BeginShaderMode(image);
        SetShaderValue(image, image_iresolution_loc, image_resolution, SHADER_UNIFORM_VEC3);
        SetShaderValueTexture(image, image_ichannel0_loc, cur_render_texture->texture);
        SetShaderValue(image, image_uamplitudescale_loc, &(float){AMPLITUDE_Y_SCALE}, SHADER_UNIFORM_FLOAT);
        SetShaderValue(image, image_urowspacing_loc, &(float){LANE_SPACING}, SHADER_UNIFORM_FLOAT);
        SetShaderValue(image, image_uisometriczoom_loc, &(float){ISOMETRIC_ZOOM}, SHADER_UNIFORM_FLOAT);
        SetShaderValue(image, image_ugridcenter_loc, image_grid_center, SHADER_UNIFORM_VEC2);
        SetShaderValue(image, image_ulinerenderwidth_loc, &image_line_render_width, SHADER_UNIFORM_FLOAT);
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
        EndShaderMode();
        EndDrawing();

        RenderTexture2D* temp_render_texture = prev_render_texture;
        prev_render_texture = cur_render_texture;
        cur_render_texture = temp_render_texture;
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
    for (int i = 0; i < WAVEFORM_BUFFER_SIZE; i++) {
        float amplitude = fabsf(waveform_window_samples[i]);
        unsigned char pixel_value = (unsigned char)(fminf(fmaxf(amplitude, 0.0f), 1.0f) * 255.0f);
        audio_texture_pixels[i].r = pixel_value;
        audio_texture_pixels[i].g = pixel_value;
        audio_texture_pixels[i].b = pixel_value;
        audio_texture_pixels[i].a = 255;
        audio_texture_pixels[i + WAVEFORM_BUFFER_SIZE].r = pixel_value;
        audio_texture_pixels[i + WAVEFORM_BUFFER_SIZE].g = pixel_value;
        audio_texture_pixels[i + WAVEFORM_BUFFER_SIZE].b = pixel_value;
        audio_texture_pixels[i + WAVEFORM_BUFFER_SIZE].a = 255;
    }
}
