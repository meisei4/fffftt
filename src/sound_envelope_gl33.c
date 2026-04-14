#include "fffftt.h"

static const char* domain = "SOUND-ENVELOPE-GL33";

#define LANE_COUNT 5
#define LANE_POINT_COUNT 64
#define WAVEFORM_SAMPLES_PER_LANE_POINT (BUFFER_SIZE / LANE_POINT_COUNT)

#define AMPLITUDE_Y_SCALE 120.0f
#define LANE_SPACING 9.0f
#define ISOMETRIC_ZOOM 3.0f
#define ISOMETRIC_GRID_CENTER_X (0.5f * (-(float)(LANE_COUNT - 1) * LANE_SPACING + (float)(LANE_POINT_COUNT - 1)))
#define ISOMETRIC_GRID_CENTER_Y (0.25f * ((float)(LANE_POINT_COUNT - 1) + (float)(LANE_COUNT - 1) * LANE_SPACING))
#define FRONT_LANE_SMOOTHING 0.4f
//TODO: shader distance threshold value, fundamentally different from a fixed-function raster unit. like ENVELOPE_LINE_WIDTH_RASTER_PIXELS
#define ENVELOPE_LINE_WIDTH_DISTANCE_PIXELS 0.5f

#define AUDIO_TEXTURE_ROW_COUNT 2

static float waveform_window_samples[WINDOW_SIZE] = {0};

static void update_audio_texture_pixels(Color* audio_texture_pixels);

int main(void) {
    int16_t chunk_samples[AUDIO_STREAM_RING_BUFFER_SIZE] = {0};

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_STREAM_RING_BUFFER_SIZE);
    Wave wave = LoadWave(RES_SHADERTOY_EXPERIMENT_22K_WAV);
    WaveFormat(&wave, SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    AudioStream stream = LoadAudioStream(SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    PlayAudioStream(stream);

    size_t wave_cursor = 0;
    int16_t* pcm_data = (int16_t*)wave.data;

    Image audio_texture_image = GenImageColor(BUFFER_SIZE, AUDIO_TEXTURE_ROW_COUNT, BLACK);
    Color* audio_texture_pixels = (Color*)audio_texture_image.data;
    Texture2D audio_texture = LoadTextureFromImage(audio_texture_image);

    Shader buffer_a = LoadShader(0, SHADER_SOUND_ENVELOPE_BUFFER_A);
    Shader image = LoadShader(0, SHADER_SOUND_ENVELOPE_IMAGE);

    int buffer_iresolution_loc = GetShaderLocation(buffer_a, "iResolution");
    int buffer_ichannel0_loc = GetShaderLocation(buffer_a, "iChannel0");
    int buffer_ichannel1_loc = GetShaderLocation(buffer_a, "iChannel1");
    int buffer_u_front_lane_smoothing_loc = GetShaderLocation(buffer_a, "u_front_lane_smoothing");

    int image_iresolution_loc = GetShaderLocation(image, "iResolution");
    int image_ichannel0_loc = GetShaderLocation(image, "iChannel0");
    int image_u_amplitude_scale_loc = GetShaderLocation(image, "u_amplitude_scale");
    int image_u_lane_spacing_loc = GetShaderLocation(image, "u_lane_spacing");
    int image_u_isometric_zoom_loc = GetShaderLocation(image, "u_isometric_zoom");
    int image_u_grid_center_loc = GetShaderLocation(image, "u_grid_center");
    int image_u_line_width_distance_pixels_loc = GetShaderLocation(image, "u_line_width_distance_pixels");

    RenderTexture2D feed = LoadRenderTexture(LANE_POINT_COUNT, LANE_COUNT);
    RenderTexture2D back = LoadRenderTexture(LANE_POINT_COUNT, LANE_COUNT);
    RenderTexture2D* prev_render_texture = &feed;
    RenderTexture2D* cur_render_texture = &back;
    float image_grid_center[2] = {
        ISOMETRIC_GRID_CENTER_X,
        ISOMETRIC_GRID_CENTER_Y,
    };

    SetTargetFPS(15); //TODO: in shadertoy this is controlled by browser config, e.g. in firefox at URL:`about:config` search: `layout.frame_rate`

    while (!WindowShouldClose()) {
        while (IsAudioStreamProcessed(stream)) {
            for (int i = 0; i < AUDIO_STREAM_RING_BUFFER_SIZE; i++) {
                chunk_samples[i] = pcm_data[wave_cursor];
                if (++wave_cursor >= wave.frameCount) {
                    wave_cursor = 0;
                }
            }

            UpdateAudioStream(stream, chunk_samples, AUDIO_STREAM_RING_BUFFER_SIZE);

            for (int i = 0; i < WINDOW_SIZE; i++) {
                waveform_window_samples[i] = (float)chunk_samples[WINDOW_SIZE + i] / PCM_SAMPLE_MAX_F;
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
        SetShaderValueTexture(buffer_a, buffer_ichannel0_loc, prev_render_texture->texture);
        SetShaderValueTexture(buffer_a, buffer_ichannel1_loc, audio_texture);
        SetShaderValue(buffer_a, buffer_u_front_lane_smoothing_loc, &(float){FRONT_LANE_SMOOTHING}, SHADER_UNIFORM_FLOAT);
        DrawRectangle(0, 0, LANE_POINT_COUNT, LANE_COUNT, WHITE);
        EndShaderMode();
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        BeginShaderMode(image);
        SetShaderValue(image, image_iresolution_loc, image_resolution, SHADER_UNIFORM_VEC3);
        SetShaderValueTexture(image, image_ichannel0_loc, cur_render_texture->texture);
        SetShaderValue(image, image_u_amplitude_scale_loc, &(float){AMPLITUDE_Y_SCALE}, SHADER_UNIFORM_FLOAT);
        SetShaderValue(image, image_u_lane_spacing_loc, &(float){LANE_SPACING}, SHADER_UNIFORM_FLOAT);
        SetShaderValue(image, image_u_isometric_zoom_loc, &(float){ISOMETRIC_ZOOM}, SHADER_UNIFORM_FLOAT);
        SetShaderValue(image, image_u_grid_center_loc, image_grid_center, SHADER_UNIFORM_VEC2);
        SetShaderValue(image, image_u_line_width_distance_pixels_loc, &(float){ENVELOPE_LINE_WIDTH_DISTANCE_PIXELS}, SHADER_UNIFORM_FLOAT);
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
    for (int i = 0; i < BUFFER_SIZE; i++) {
        float amplitude = fabsf(waveform_window_samples[(i * (WINDOW_SIZE - 1)) / (BUFFER_SIZE - 1)]);
        unsigned char pixel_value = (unsigned char)(fminf(fmaxf(amplitude, 0.0f), 1.0f) * 255.0f);
        audio_texture_pixels[i].r = pixel_value;
        audio_texture_pixels[i].g = pixel_value;
        audio_texture_pixels[i].b = pixel_value;
        audio_texture_pixels[i].a = 255;
        audio_texture_pixels[i + BUFFER_SIZE].r = pixel_value;
        audio_texture_pixels[i + BUFFER_SIZE].g = pixel_value;
        audio_texture_pixels[i + BUFFER_SIZE].b = pixel_value;
        audio_texture_pixels[i + BUFFER_SIZE].a = 255;
    }
}
