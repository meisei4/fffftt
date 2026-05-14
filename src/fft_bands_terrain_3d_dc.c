#define FFFFTT_PROFILE_FFT_TERRAIN_3D
#include "fffftt.h"
#include <GL/gl.h>

#define LINE_WIDTH_RASTER_PIXELS 1.0f
#define POINT_SIZE_RASTER_PIXELS 3.0f
#define WHITE_NOISE_TEXELS_PER_QUAD 8

// https://en.wikipedia.org/wiki/12_equal_temperament
// TODO: look up tables to make it clear what the bands actually consist of,
// NOTE: THESE ARE MANUALLY WRITTEN AND SHOULD STAY IN ALIGNMENT WITH ANY CHANGES TO THE ABOVE *_BAND_POINT_COUNT DENSITY POLICY!
// #define LOW_BAND_POINT_COUNT 44  // CEIL(12 * LOG2(250 / 20)) = 44 -> divide by two -> 22 but then low has too much reduncancy to take the 10 duplicates and send them to the high band for more noise
// #define MID_BAND_POINT_COUNT 48  // CEIL(12 * LOG2(4000 / 250)) = 48, -> divide by two -> 24
// #define HIGH_BAND_POINT_COUNT 18 // CEIL(12 * LOG2(11025 / 4000)) = 18 -> divide by two -> 9 add 10 from the low band...
#define LOW_BAND_POINT_COUNT 12
#define MID_BAND_POINT_COUNT 24
#define HIGH_BAND_POINT_COUNT 19
static const unsigned short LOW_BAND_BIN_BOUNDS[LOW_BAND_POINT_COUNT][2] = {
    {1, 1},
    {2, 2},
    {3, 3},
    {4, 4},
    {5, 5},
    {6, 6},
    {7, 7},
    {8, 8},
    {9, 9},
    {10, 10},
    {11, 11},
    {12, 12},
};

static const unsigned short MID_BAND_BIN_BOUNDS[MID_BAND_POINT_COUNT][2] = {
    {13, 13}, {14, 15}, {16, 17}, {18, 19}, {20, 21}, {22, 24}, {25, 27},  {28, 30},   {31, 34},   {35, 38},   {39, 43},   {44, 48},
    {49, 53}, {54, 60}, {61, 67}, {68, 75}, {76, 84}, {85, 94}, {95, 105}, {106, 118}, {119, 132}, {133, 148}, {149, 165}, {166, 185},
};

static const unsigned short HIGH_BAND_BIN_BOUNDS[HIGH_BAND_POINT_COUNT][2] = {
    {186, 202}, {203, 219}, {220, 236}, {237, 253}, {254, 270}, {271, 287}, {288, 305}, {306, 322}, {323, 339}, {340, 356},
    {357, 373}, {374, 390}, {391, 408}, {409, 425}, {426, 442}, {443, 459}, {460, 476}, {477, 493}, {494, 511},
};

#define LOW_BAND_VERTEX_COUNT (LANE_COUNT * LOW_BAND_POINT_COUNT)
#define MID_BAND_VERTEX_COUNT (LANE_COUNT * MID_BAND_POINT_COUNT)
#define HIGH_BAND_VERTEX_COUNT (LANE_COUNT * HIGH_BAND_POINT_COUNT)

#define MID_BAND_TERRAIN_TRIANGLE_COUNT (((LANE_COUNT - 1) * (MID_BAND_POINT_COUNT - 1)) * 2)
#define FLAT_MID_BAND_VERTEX_COUNT (MID_BAND_TERRAIN_TRIANGLE_COUNT * 3)

#define ADAPTIVE_PEAK_DECAY 0.985f
#define ADAPTIVE_DYNAMIC_RANGE_DB 40.0f
#define LOW_BAND_Y_SCALE 1.5f
#define MID_BAND_Y_SCALE 3.0f
#define HIGH_BAND_Y_SCALE 1.0f
#define LOW_BAND_ANCHOR (Vector3){0.0f, -LOW_BAND_Y_SCALE, 0.0f}
#define MID_BAND_ANCHOR (Vector3){0.0f, 0.0f, 0.0f}
#define HIGH_BAND_ANCHOR (Vector3){0.0f, MID_BAND_Y_SCALE, 0.0f}

static const char* domain = "FFT-BANDS-TERRAIN-3D-DC";

static Mesh low_band_mesh = {0};
static Mesh mid_band_mesh = {0};
static Mesh high_band_mesh = {0};
static Mesh flat_mesh = {0};

static Model low_band_model = {0};
static Model mid_band_model = {0};
static Model high_band_model = {0};
static Model flat_model = {0};

static float low_band_lane_point_values[LANE_COUNT][LOW_BAND_POINT_COUNT] = {0};
static float mid_band_lane_point_values[LANE_COUNT][MID_BAND_POINT_COUNT] = {0};
static float high_band_lane_point_values[LANE_COUNT][HIGH_BAND_POINT_COUNT] = {0};

static float low_band_vertices[LOW_BAND_VERTEX_COUNT * 3] = {0};
static float low_band_normals[LOW_BAND_VERTEX_COUNT * 3] = {0};
static Color low_band_colors[LOW_BAND_VERTEX_COUNT] = {0};
static float low_band_texcoords[LOW_BAND_VERTEX_COUNT * 2] = {0};

static float mid_band_vertices[MID_BAND_VERTEX_COUNT * 3] = {0};
static float mid_band_normals[MID_BAND_VERTEX_COUNT * 3] = {0};
static Color mid_band_colors[MID_BAND_VERTEX_COUNT] = {0};
static float mid_band_texcoords[MID_BAND_VERTEX_COUNT * 2] = {0};
static unsigned char mid_band_lane_chroma_id[LANE_COUNT] = {0};
static float mid_band_chroma_mask_field[LANE_COUNT][MID_BAND_POINT_COUNT] = {0};

static float high_band_vertices[HIGH_BAND_VERTEX_COUNT * 3] = {0};
static float high_band_normals[HIGH_BAND_VERTEX_COUNT * 3] = {0};
static Color high_band_colors[HIGH_BAND_VERTEX_COUNT] = {0};
static float high_band_texcoords[HIGH_BAND_VERTEX_COUNT * 2] = {0};
static float high_band_spectral_flatness_glitter_field[HIGH_BAND_VERTEX_COUNT] = {0};
static float spectral_flatness_glitter_alpha = 0.0f;
static float spectral_flatness_glitter_alpha_history[ANALYSIS_FFT_HISTORY_FRAME_COUNT] = {0};
static float adaptive_glitter_power_mean = 0.0f;
static float adaptive_glitter_power_deviation = 0.0f;
static int adaptive_glitter_power_ready = 0;

static float flat_vertices[FLAT_MID_BAND_VERTEX_COUNT * 3] = {0};
static float flat_normals[FLAT_MID_BAND_VERTEX_COUNT * 3] = {0};
static Color flat_colors[FLAT_MID_BAND_VERTEX_COUNT] = {0};

static void build_band_terrain(float* lane_point_values, int point_count, const float* levels, const unsigned short band_bin_bounds[][2], float* adaptive_peak);
static void update_spectral_flatness_glitter_alpha(const float* raw_spectrum_magnitudes);
static void update_mesh_colors_spectral_flatness_glitter(Color* colors, const float* glitter_field);

static int inspection_ready = 0;
static int cur_frame_pos = 0;
static int retention_window_min_frame_pos = 0;
static int retention_window_max_frame_pos = 0;
static void update_playback_controls_fft(void);
static void consume_current_fft_frame(void);
static void build_fft_terrain_lane_from_frame_pos(int lane, int frame_pos);
static void inspection_step(int dir);
static void update_fft_bands_terrain_meshes(void);
static void rebase_fft_history(void);

#define FFT_BANDS_SCENE_BBOX                                                                                                                                   \
    ((BoundingBox){                                                                                                                                            \
        .min = {-LIGHT_SCENE_X_HALF, -LOW_BAND_Y_SCALE, -LIGHT_SCENE_Z_HALF},                                                                                  \
        .max = {+LIGHT_SCENE_X_HALF, MID_BAND_Y_SCALE + HIGH_BAND_Y_SCALE, +LIGHT_SCENE_Z_HALF},                                                               \
    })

int main(void) {
    // SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    font = LoadFont(RD_FONT);
    fft_data.tapback_pos = ANALYSIS_TAPBACK_POS_DEFAULT;
    fft_data.work_buffer = RL_CALLOC(ANALYSIS_WINDOW_SIZE_IN_FRAMES, sizeof(FFTComplex));
    fft_data.smoothed_spectrum_magnitudes = RL_CALLOC(ANALYSIS_SPECTRUM_BIN_COUNT, sizeof(float));
    fft_data.raw_spectrum_magnitudes = RL_CALLOC(ANALYSIS_FFT_HISTORY_FRAME_COUNT, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]));
    fft_data.spectrum_levels = RL_CALLOC(ANALYSIS_FFT_HISTORY_FRAME_COUNT, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]));

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
    set_audio_track(SHADERTOY_EXPERIMENT);
    audio_stream = LoadAudioStream(SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    PlayAudioStream(audio_stream);

    Camera3D camera = {
        .position = (Vector3){1.45625f * 3.0f, 1.345f * 3.0f, -1.36625f * 3.0f},
        .target = (Vector3){0.0f, 1.75f, 0.0f},
        .up = Y_AXIS,
        .fovy = 8.0f,
        .projection = CAMERA_ORTHOGRAPHIC,
    };

    low_band_mesh = GenMeshPlane(1.0f, 1.0f, LOW_BAND_POINT_COUNT - 1, LANE_COUNT - 1);
    low_band_mesh.colors = RL_CALLOC(low_band_mesh.vertexCount, sizeof(Color));
    int* low_band_white_noise_dimensions = WHITE_NOISE_MASK_DIMS(LOW_BAND_POINT_COUNT, WHITE_NOISE_TEXELS_PER_QUAD);
    Texture2D low_band_white_noise_texture = build_white_noise_mask(
        low_band_white_noise_dimensions[0], low_band_white_noise_dimensions[1], low_band_texcoords, LOW_BAND_POINT_COUNT, WHITE_NOISE_TEXELS_PER_QUAD * 8);
    low_band_model = LoadModelFromMesh(low_band_mesh);
    unsigned char* low_saved_colors = low_band_model.meshes[0].colors;

    mid_band_mesh = GenMeshPlane(1.0f, 1.0f, MID_BAND_POINT_COUNT - 1, LANE_COUNT - 1);
    mid_band_mesh.colors = RL_CALLOC(mid_band_mesh.vertexCount, sizeof(Color));
    mid_band_mesh.texcoords2 = RL_CALLOC(mid_band_mesh.vertexCount * 2, sizeof(float));
    Texture2D lane_mask_texture = build_lane_mask(mid_band_mesh.texcoords2, MID_BAND_POINT_COUNT);
    wave_cursor_texture = build_lane_mask_glow(mid_band_mesh.texcoords2, MID_BAND_POINT_COUNT);
    int* mid_band_white_noise_dimensions = WHITE_NOISE_MASK_DIMS(MID_BAND_POINT_COUNT, WHITE_NOISE_TEXELS_PER_QUAD);
    Texture2D mid_band_white_noise_texture = build_white_noise_mask(
        mid_band_white_noise_dimensions[0], mid_band_white_noise_dimensions[1], mid_band_texcoords, MID_BAND_POINT_COUNT, WHITE_NOISE_TEXELS_PER_QUAD * 4);
    mid_band_model = LoadModelFromMesh(mid_band_mesh);
    unsigned char* mid_saved_colors = mid_band_model.meshes[0].colors;
    mid_band_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = lane_mask_texture;
    mid_band_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = 0;

    high_band_mesh = GenMeshPlane(1.0f, 1.0f, HIGH_BAND_POINT_COUNT - 1, LANE_COUNT - 1);
    high_band_mesh.colors = RL_CALLOC(high_band_mesh.vertexCount, sizeof(Color));
    int* high_band_white_noise_dimensions = WHITE_NOISE_MASK_DIMS(HIGH_BAND_POINT_COUNT, WHITE_NOISE_TEXELS_PER_QUAD);
    Texture2D high_band_white_noise_texture = build_white_noise_mask(
        high_band_white_noise_dimensions[0], high_band_white_noise_dimensions[1], high_band_texcoords, HIGH_BAND_POINT_COUNT, WHITE_NOISE_TEXELS_PER_QUAD);
    high_band_model = LoadModelFromMesh(high_band_mesh);
    unsigned char* high_saved_colors = high_band_model.meshes[0].colors;
    Color spectral_flatness_glitter_tint = WHITE;
    int white_noise_texture_id = high_band_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id;
    high_band_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = 0;

    flat_mesh = (Mesh){
        .vertexCount = mid_band_mesh.triangleCount * 3,
        .triangleCount = mid_band_mesh.triangleCount,
        .vertices = RL_CALLOC(mid_band_mesh.triangleCount * 3 * 3, sizeof(float)),
        .normals = RL_CALLOC(mid_band_mesh.triangleCount * 3 * 3, sizeof(float)),
        .colors = RL_CALLOC(mid_band_mesh.triangleCount * 3, sizeof(Color)),
        .indices = NULL, //NOTE: explicit here for didactic
    };
    flat_model = LoadModelFromMesh(flat_mesh);

    wave_cursor_model = &mid_band_model;

    fill_mesh_colors(low_band_colors, LOW_BAND_POINT_COUNT);
    // update_mesh_colors_jet_scroll(low_band_colors, LOW_BAND_POINT_COUNT, wave_cursor);
    fill_mesh_colors(mid_band_colors, MID_BAND_POINT_COUNT);
    fill_mesh_colors(high_band_colors, HIGH_BAND_POINT_COUNT);

    update_fft_bands_terrain_meshes();
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        update_audio_track_cycle();
        update_playback_controls_fft();

        int audio_dirty = 0;
        while (fffftt_audio_process(chunk_samples)) {
            apply_blackman_window();
            shz_fft((shz_complex_t*)fft_data.work_buffer, (size_t)ANALYSIS_WINDOW_SIZE_IN_FRAMES);
            build_spectrum();
            consume_current_fft_frame();
            audio_dirty = 1;
        }

        if (audio_dirty) {
            update_fft_bands_terrain_meshes();
        }
        update_mesh_texcoords_smooth_scroll(high_band_white_noise_dimensions[0],
                                            high_band_white_noise_dimensions[1],
                                            high_band_mesh.texcoords,
                                            HIGH_BAND_POINT_COUNT,
                                            WHITE_NOISE_TEXELS_PER_QUAD,
                                            (Vector2){1.0f, 0.25f},
                                            0.05f,
                                            (float)GetTime());
        // update_mesh_texcoords_smooth_scroll(low_band_white_noise_dimensions[0],
        //                                     low_band_white_noise_dimensions[1],
        //                                     low_band_mesh.texcoords,
        //                                     LOW_BAND_POINT_COUNT,
        //                                     WHITE_NOISE_TEXELS_PER_QUAD * 8,
        //                                     (Vector2){1.0f, 0.25f},
        //                                     0.05f,
        //                                     (float)GetTime());
        update_mesh_texcoords_smooth_scroll(mid_band_white_noise_dimensions[0],
                                            mid_band_white_noise_dimensions[1],
                                            mid_band_mesh.texcoords,
                                            MID_BAND_POINT_COUNT,
                                            WHITE_NOISE_TEXELS_PER_QUAD * 4,
                                            (Vector2){-1.0f, -0.25f},
                                            0.09f,
                                            (float)GetTime());
        update_camera_orbit(&camera, GetFrameTime());
        update_diffuse_strength();
        update_light_camera_strafe(&camera, compute_buoy_rest_pos(low_band_vertices, LOW_BAND_POINT_COUNT, LOW_BAND_ANCHOR), FFT_BANDS_SCENE_BBOX);

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);
        rlSetLineWidth(LINE_WIDTH_RASTER_PIXELS);
        rlSetPointSize(POINT_SIZE_RASTER_PIXELS);

        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (const GLfloat[]){0.18f, 0.18f, 0.18f, 1.0f});
        glLightfv(GL_LIGHT0, GL_AMBIENT, (const GLfloat[]){0.0f, 0.0f, 0.0f, 1.0f});
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
        glLightfv(GL_LIGHT0, GL_POSITION, (const GLfloat[]){light0_pos.x, light0_pos.y, light0_pos.z, 1.0f});
        DrawModelEx(low_band_model, LOW_BAND_ANCHOR, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE); //TODO: too powerful...
        glDisable(GL_LIGHTING);
        draw_lantern(light0_pos);
        draw_lantern_glow(light0_pos);

        glEnable(GL_LIGHTING);
        glShadeModel(GL_FLAT);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (const GLfloat[]){0.0f, 0.0f, 0.0f, 1.0f});
        glLightfv(GL_LIGHT0, GL_AMBIENT, (const GLfloat[]){0.0f, 0.0f, 0.0f, 1.0f});
        glLightfv(GL_LIGHT0, GL_DIFFUSE, (const GLfloat[]){1.0f, 1.0f, 1.0f, 1.0f});
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, (const GLfloat[]){0.0f, 0.0f, 0.0f, 0.0f});
        DrawModelEx(flat_model, MID_BAND_ANCHOR, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE); //TODO: too powerful...
        glShadeModel(GL_SMOOTH);
        glDisable(GL_LIGHTING);

        low_band_model.meshes[0].colors = NULL;
        mid_band_model.meshes[0].colors = NULL;
        high_band_model.meshes[0].colors = NULL;
        high_band_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = high_band_white_noise_texture.id;
        mid_band_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = mid_band_white_noise_texture.id;
        spectral_flatness_glitter_tint.a = (unsigned char)(spectral_flatness_glitter_alpha * (float)DRAW_COLOR_CHANNEL_MAX);
        if (spectral_flatness_glitter_tint.a != 0) {
            rlEnablePointMode();
            rlDisableDepthTest();
            DrawModelEx(high_band_model, HIGH_BAND_ANCHOR, Y_AXIS, 0.0f, DEFAULT_SCALE, spectral_flatness_glitter_tint);
            rlEnableDepthTest();
            rlDisablePointMode();
            DrawModelWiresEx(mid_band_model, MID_BAND_ANCHOR, Y_AXIS, 0.0f, DEFAULT_SCALE, spectral_flatness_glitter_tint);
        }
        low_band_model.meshes[0].colors = low_saved_colors;
        mid_band_model.meshes[0].colors = mid_saved_colors;
        high_band_model.meshes[0].colors = high_saved_colors;

        if (is_paused) {
            draw_paused_wave_cursor_lane_marker(MID_BAND_POINT_COUNT);
        }

        EndMode3D();
        draw_playback_inspection_hud();
        EndDrawing();
    }

    UnloadTexture(lane_mask_texture);
    UnloadTexture(wave_cursor_texture);
    UnloadTexture(high_band_white_noise_texture);
    UnloadTexture(mid_band_white_noise_texture);
    UnloadTexture(low_band_white_noise_texture);
    UnloadModel(low_band_model);
    UnloadModel(mid_band_model);
    UnloadModel(high_band_model);
    UnloadModel(flat_model);
    UnloadAudioStream(audio_stream);
    unload_audio_track();
    CloseAudioDevice();
    RL_FREE(fft_data.raw_spectrum_magnitudes);
    RL_FREE(fft_data.spectrum_levels);
    RL_FREE(fft_data.smoothed_spectrum_magnitudes);
    RL_FREE(fft_data.work_buffer);
    UnloadFont(font);
    CloseWindow();
    return 0;
}

static float low_band_adaptive_peak = 0.0f;
static float mid_band_adaptive_peak = 0.0f;
static float high_band_adaptive_peak = 0.0f;

static void
build_band_terrain(float* lane_point_values, int point_count, const float* levels, const unsigned short band_bin_bounds[][2], float* adaptive_peak) {
    float band_peak = 0.0f;
    for (int i = 0; i < point_count; i++) {
        int bin_min = band_bin_bounds[i][0];
        int bin_max = band_bin_bounds[i][1];
        float magnitude_sum = 0.0f;
        for (int j = bin_min; j <= bin_max; j++) {
            magnitude_sum += levels[j];
        }
        float avg_magnitude = magnitude_sum / (float)(bin_max - bin_min + 1);
        lane_point_values[i] = avg_magnitude;
        band_peak = FMAXF(band_peak, avg_magnitude);
    }

    if (adaptive_peak) {
        *adaptive_peak = FMAXF(*adaptive_peak * ADAPTIVE_PEAK_DECAY, band_peak);
        float inv_peak = 1.0f / *adaptive_peak;
        float inv_range_db = 1.0f / ADAPTIVE_DYNAMIC_RANGE_DB;
        for (int i = 0; i < point_count; i++) {
            float db_below_peak = LOGF(lane_point_values[i] * inv_peak) * ANALYSIS_DB_TO_LINEAR_SCALE;
            lane_point_values[i] = CLAMP(1.0f + db_below_peak * inv_range_db, 0.0f, 1.0f);
        }
    } else {
        for (int i = 0; i < point_count; i++) {
            lane_point_values[i] = CLAMP(lane_point_values[i], 0.0f, 1.0f);
        }
    }
}

// https://librosa.org/doc/main/_modules/librosa/feature/spectral.html#spectral_flatness
#define SPECTRAL_FLATNESS_AMIN 1.0e-10f // amin: float = 1e-10,

static void update_spectral_flatness_glitter_alpha(const float* raw_spectrum_magnitudes) {
    // https://librosa.org/doc/main/generated/librosa.feature.spectral_flatness.html
    const int bin_min = HIGH_BAND_BIN_BOUNDS[0][0];
    const int bin_max = HIGH_BAND_BIN_BOUNDS[HIGH_BAND_POINT_COUNT - 1][1];
    const int bin_count = bin_max - bin_min + 1;
    float log_power_sum = 0.0f;
    float arithmetic_power_sum = 0.0f;

    for (int i = bin_min; i <= bin_max; i++) {
        const float power = FMAXF(raw_spectrum_magnitudes[i] * raw_spectrum_magnitudes[i], SPECTRAL_FLATNESS_AMIN);
        log_power_sum += LOGF(power);
        arithmetic_power_sum += power;
    }
    const float geometric_mean = EXPF(log_power_sum / (float)bin_count);
    const float arithmetic_mean = arithmetic_power_sum / (float)bin_count;
    if (!adaptive_glitter_power_ready) {
        adaptive_glitter_power_mean = arithmetic_mean;
        adaptive_glitter_power_deviation = arithmetic_mean;
        adaptive_glitter_power_ready = 1;
    } else {
        float adaptive_glitter_power_delta = FABSF(arithmetic_mean - adaptive_glitter_power_mean);
        adaptive_glitter_power_mean = LERP(adaptive_glitter_power_mean, arithmetic_mean, GLOBAL_ADAPTIVE_GATE_SMOOTHING_FACTOR);
        adaptive_glitter_power_deviation = LERP(adaptive_glitter_power_deviation, adaptive_glitter_power_delta, GLOBAL_ADAPTIVE_GATE_SMOOTHING_FACTOR);
    }
    float adaptive_glitter_power_floor = adaptive_glitter_power_mean - adaptive_glitter_power_deviation * GLOBAL_ADAPTIVE_GATE_FLOOR_DEVIATION_SCALE;
    float adaptive_glitter_power_ceiling = adaptive_glitter_power_mean + adaptive_glitter_power_deviation * GLOBAL_ADAPTIVE_GATE_CEILING_DEVIATION_SCALE;
    const float spectral_flatness = geometric_mean / FMAXF(arithmetic_mean, SPECTRAL_FLATNESS_AMIN);
    float flatness = CLAMP(spectral_flatness, 0.0f, 1.0f);
    float gate = CLAMP((arithmetic_mean - adaptive_glitter_power_floor) / (adaptive_glitter_power_ceiling - adaptive_glitter_power_floor), 0.0f, 1.0f);
    gate = gate * gate * (3.0f - 2.0f * gate);
    float alpha_target = CLAMP(flatness * gate, 0.0f, 1.0f);
    spectral_flatness_glitter_alpha = alpha_target;
    spectral_flatness_glitter_alpha_history[WRAP_MINUS(fft_data.history_frame_pos, 1, ANALYSIS_FFT_HISTORY_FRAME_COUNT)] = spectral_flatness_glitter_alpha;
    //TODO: and then do this for anticipating a distributed field from a 1D signal measurement... later...
    for (int i = 0; i < HIGH_BAND_VERTEX_COUNT; i++) {
        high_band_spectral_flatness_glitter_field[i] = spectral_flatness_glitter_alpha;
    }
}

static void update_mesh_colors_spectral_flatness_glitter(Color* colors, const float* glitter_field) {
    for (int i = 0; i < HIGH_BAND_VERTEX_COUNT; i++) {
        colors[i] = WHITE;
        colors[i].a = (unsigned char)(glitter_field[i] * (float)DRAW_COLOR_CHANNEL_MAX);
    }
}

static void consume_current_fft_frame(void) {
    int cur_history_frame_pos = WRAP_MINUS(fft_data.history_frame_pos, 1, ANALYSIS_FFT_HISTORY_FRAME_COUNT);
    float* spectrum_levels = fft_data.spectrum_levels[cur_history_frame_pos];
    float* raw_spectrum_magnitudes = fft_data.raw_spectrum_magnitudes[cur_history_frame_pos];
    advance_lane_history(&low_band_lane_point_values[0][0], LOW_BAND_POINT_COUNT);
    advance_lane_history(&mid_band_lane_point_values[0][0], MID_BAND_POINT_COUNT);
    advance_lane_history(&high_band_lane_point_values[0][0], HIGH_BAND_POINT_COUNT);
    advance_lane_history_u8(mid_band_lane_chroma_id, 1);
    advance_lane_history(&mid_band_chroma_mask_field[0][0], MID_BAND_POINT_COUNT);
    build_band_terrain(&low_band_lane_point_values[0][0], LOW_BAND_POINT_COUNT, spectrum_levels, LOW_BAND_BIN_BOUNDS, NULL);
    // build_band_terrain(&low_band_lane_point_values[0][0], LOW_BAND_POINT_COUNT, raw_spectrum_magnitudes, LOW_BAND_BIN_BOUNDS, &low_band_adaptive_peak);
    build_band_terrain(&mid_band_lane_point_values[0][0], MID_BAND_POINT_COUNT, raw_spectrum_magnitudes, MID_BAND_BIN_BOUNDS, &mid_band_adaptive_peak);
    build_band_terrain(&high_band_lane_point_values[0][0], HIGH_BAND_POINT_COUNT, raw_spectrum_magnitudes, HIGH_BAND_BIN_BOUNDS, &high_band_adaptive_peak);
    build_chroma_fields(&mid_band_lane_chroma_id[0],
                        &mid_band_chroma_mask_field[0][0],
                        &mid_band_lane_point_values[0][0],
                        raw_spectrum_magnitudes,
                        MID_BAND_POINT_COUNT,
                        1,
                        ANALYSIS_SPECTRUM_BIN_COUNT - 1);
    update_onset_gate_fft(&fft_data, 1);
    update_spectral_flatness_glitter_alpha(raw_spectrum_magnitudes);
}

static void update_playback_controls_fft(void) {
    int analysis_dirty = 0;

    if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
        reset_sticky_nav();
        if (!is_paused) {
            is_paused = true;
            wave_cursor = WRAP_MINUS(wave_cursor, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES, wave.frameCount);
            paused_wave_cursor = wave_cursor;
            seek_delta_chunks = 0;
            fffftt_audio_drain();
            PauseAudioStream(audio_stream);
            rebase_fft_history();
            update_fft_bands_terrain_meshes();
        } else {
            is_paused = false;
            inspection_ready = 0;
            PlayAudioStream(audio_stream);
            while (fffftt_audio_process(resume_chunk_samples)) {
                apply_blackman_window();
                shz_fft((shz_complex_t*)fft_data.work_buffer, (size_t)ANALYSIS_WINDOW_SIZE_IN_FRAMES);
                build_spectrum();
                consume_current_fft_frame();
                analysis_dirty = 1;
            }
        }
    }

    if (is_paused && sticky_nav(GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
        wave_cursor = WRAP_MINUS(wave_cursor, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES, wave.frameCount);
        seek_delta_chunks--;
        int target_frame_pos = cur_frame_pos - 1;
        if (inspection_ready && cur_frame_pos > 0 && target_frame_pos >= retention_window_min_frame_pos + (LANE_COUNT - 1)) {
            cur_frame_pos = target_frame_pos;
            inspection_step(BACKWARD);
        } else {
            rebase_fft_history();
        }
        int cur_history_frame_pos = WRAP(cur_frame_pos, ANALYSIS_FFT_HISTORY_FRAME_COUNT);
        onset_gate = onset_gate_history[cur_history_frame_pos];
        spectral_flatness_glitter_alpha = spectral_flatness_glitter_alpha_history[cur_history_frame_pos];
        for (int i = 0; i < HIGH_BAND_VERTEX_COUNT; i++) {
            high_band_spectral_flatness_glitter_field[i] = spectral_flatness_glitter_alpha;
        }
        update_fft_bands_terrain_meshes();
    } else if (is_paused && sticky_nav(GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
        wave_cursor = WRAP_PLUS(wave_cursor, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES, wave.frameCount);
        seek_delta_chunks++;
        int target_frame_pos = cur_frame_pos + 1;
        if (inspection_ready && target_frame_pos <= retention_window_max_frame_pos) {
            cur_frame_pos = target_frame_pos;
            inspection_step(FORWARD);
        } else if (inspection_ready && cur_frame_pos == retention_window_max_frame_pos) {
            fffftt_inspection_fill_analysis_window(wave_cursor);

            apply_blackman_window();
            shz_fft((shz_complex_t*)fft_data.work_buffer, (size_t)ANALYSIS_WINDOW_SIZE_IN_FRAMES);
            build_spectrum();

            int frame_pos = fft_data.frame_pos - 1;
            update_onset_gate_fft(&fft_data, 1);
            update_spectral_flatness_glitter_alpha(
                fft_data.raw_spectrum_magnitudes[WRAP_MINUS(fft_data.history_frame_pos, 1, ANALYSIS_FFT_HISTORY_FRAME_COUNT)]);
            retention_window_max_frame_pos = frame_pos;
            if (retention_window_max_frame_pos - retention_window_min_frame_pos >= ANALYSIS_FFT_HISTORY_FRAME_COUNT) {
                retention_window_min_frame_pos = retention_window_max_frame_pos - (ANALYSIS_FFT_HISTORY_FRAME_COUNT - 1);
            }
            cur_frame_pos = retention_window_max_frame_pos;
            inspection_step(FORWARD);
        } else {
            rebase_fft_history();
        }
        int cur_history_frame_pos = WRAP(cur_frame_pos, ANALYSIS_FFT_HISTORY_FRAME_COUNT);
        onset_gate = onset_gate_history[cur_history_frame_pos];
        spectral_flatness_glitter_alpha = spectral_flatness_glitter_alpha_history[cur_history_frame_pos];
        for (int i = 0; i < HIGH_BAND_VERTEX_COUNT; i++) {
            high_band_spectral_flatness_glitter_field[i] = spectral_flatness_glitter_alpha;
        }
        update_fft_bands_terrain_meshes();
    }

    if (analysis_dirty) {
        update_fft_bands_terrain_meshes();
    }
}

static void build_fft_terrain_lane_from_frame_pos(int lane, int frame_pos) {
    if (frame_pos < retention_window_min_frame_pos || frame_pos > retention_window_max_frame_pos) {
        MEMSET(low_band_lane_point_values[lane], 0, sizeof(low_band_lane_point_values[lane]));
        MEMSET(mid_band_lane_point_values[lane], 0, sizeof(mid_band_lane_point_values[lane]));
        MEMSET(high_band_lane_point_values[lane], 0, sizeof(high_band_lane_point_values[lane]));
        mid_band_lane_chroma_id[lane] = 0;
        MEMSET(mid_band_chroma_mask_field[lane], 0, sizeof(mid_band_chroma_mask_field[lane]));
        return;
    }

    int history_frame_pos = WRAP(frame_pos, ANALYSIS_FFT_HISTORY_FRAME_COUNT);
    float* spectrum_levels = fft_data.spectrum_levels[history_frame_pos];
    float* raw_spectrum_magnitudes = fft_data.raw_spectrum_magnitudes[history_frame_pos];
    build_band_terrain(&low_band_lane_point_values[lane][0], LOW_BAND_POINT_COUNT, spectrum_levels, LOW_BAND_BIN_BOUNDS, NULL);
    // build_band_terrain(&low_band_lane_point_values[lane][0], LOW_BAND_POINT_COUNT, raw_spectrum_magnitudes, LOW_BAND_BIN_BOUNDS, &low_band_adaptive_peak);
    build_band_terrain(&mid_band_lane_point_values[lane][0], MID_BAND_POINT_COUNT, raw_spectrum_magnitudes, MID_BAND_BIN_BOUNDS, &mid_band_adaptive_peak);
    build_band_terrain(&high_band_lane_point_values[lane][0], HIGH_BAND_POINT_COUNT, raw_spectrum_magnitudes, HIGH_BAND_BIN_BOUNDS, &high_band_adaptive_peak);
    build_chroma_fields(&mid_band_lane_chroma_id[lane],
                        &mid_band_chroma_mask_field[lane][0],
                        &mid_band_lane_point_values[lane][0],
                        raw_spectrum_magnitudes,
                        MID_BAND_POINT_COUNT,
                        1,
                        ANALYSIS_SPECTRUM_BIN_COUNT - 1);
}

static void inspection_step(int dir) {
    for (int i = (dir == FORWARD) ? LANE_COUNT - 1 : 0; i != ((dir == FORWARD) ? 0 : LANE_COUNT - 1); i -= dir) {
        MEMCPY(lane_point_values[i], lane_point_values[i - dir], sizeof(lane_point_values[i]));
        MEMCPY(low_band_lane_point_values[i], low_band_lane_point_values[i - dir], sizeof(low_band_lane_point_values[i]));
        MEMCPY(mid_band_lane_point_values[i], mid_band_lane_point_values[i - dir], sizeof(mid_band_lane_point_values[i]));
        MEMCPY(high_band_lane_point_values[i], high_band_lane_point_values[i - dir], sizeof(high_band_lane_point_values[i]));
        mid_band_lane_chroma_id[i] = mid_band_lane_chroma_id[i - dir];
        MEMCPY(mid_band_chroma_mask_field[i], mid_band_chroma_mask_field[i - dir], sizeof(mid_band_chroma_mask_field[i]));
    }
    build_fft_terrain_lane_from_frame_pos((dir == FORWARD) ? 0 : LANE_COUNT - 1, (dir == FORWARD) ? cur_frame_pos : cur_frame_pos - (LANE_COUNT - 1));
}

static void update_fft_bands_terrain_meshes(void) {
    update_mesh_vertices(low_band_vertices, &low_band_lane_point_values[0][0], LOW_BAND_POINT_COUNT, LOW_BAND_Y_SCALE);
    update_mesh_normals_smooth(low_band_normals, low_band_vertices, LOW_BAND_POINT_COUNT);
    // update_mesh_colors_jet_scroll(low_band_colors, LOW_BAND_POINT_COUNT, wave_cursor);

    update_mesh_vertices(mid_band_vertices, &mid_band_lane_point_values[0][0], MID_BAND_POINT_COUNT, MID_BAND_Y_SCALE);
    update_mesh_normals_smooth(mid_band_normals, mid_band_vertices, MID_BAND_POINT_COUNT);
    update_mesh_colors_chroma(mid_band_colors, mid_band_lane_chroma_id, &mid_band_chroma_mask_field[0][0], MID_BAND_POINT_COUNT);

    update_mesh_vertices(high_band_vertices, &high_band_lane_point_values[0][0], HIGH_BAND_POINT_COUNT, HIGH_BAND_Y_SCALE);
    update_mesh_normals_smooth(high_band_normals, high_band_vertices, HIGH_BAND_POINT_COUNT);
    update_mesh_colors_spectral_flatness_glitter(high_band_colors, high_band_spectral_flatness_glitter_field);

    expand_mesh_colors_flat(flat_colors, mid_band_colors, mid_band_mesh.indices, FLAT_MID_BAND_VERTEX_COUNT);
    update_mesh_vertices_flat(flat_vertices, mid_band_vertices, mid_band_mesh.indices, FLAT_MID_BAND_VERTEX_COUNT);
    update_mesh_normals_flat(flat_normals, flat_vertices, MID_BAND_TERRAIN_TRIANGLE_COUNT);

    build_mesh_smooth(&low_band_mesh, low_band_vertices, low_band_normals, low_band_colors, low_band_mesh.texcoords, LOW_BAND_VERTEX_COUNT);
    build_mesh_smooth(&mid_band_mesh, mid_band_vertices, mid_band_normals, mid_band_colors, mid_band_texcoords, MID_BAND_VERTEX_COUNT);
    build_mesh_smooth(&high_band_mesh, high_band_vertices, high_band_normals, high_band_colors, high_band_texcoords, HIGH_BAND_VERTEX_COUNT);
    build_mesh_flat(&flat_mesh, flat_vertices, flat_normals, flat_colors, FLAT_MID_BAND_VERTEX_COUNT);
}

static void rebase_fft_history(void) {
    inspection_ready = 0;
    MEMSET(fft_data.smoothed_spectrum_magnitudes, 0, sizeof(float) * ANALYSIS_SPECTRUM_BIN_COUNT);
    MEMSET(fft_data.raw_spectrum_magnitudes, 0, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]) * ANALYSIS_FFT_HISTORY_FRAME_COUNT);
    MEMSET(fft_data.spectrum_levels, 0, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]) * ANALYSIS_FFT_HISTORY_FRAME_COUNT);
    MEMSET(onset_gate_history, 0, sizeof(onset_gate_history));
    MEMSET(spectral_flatness_glitter_alpha_history, 0, sizeof(spectral_flatness_glitter_alpha_history));
    MEMSET(low_band_lane_point_values, 0, sizeof(low_band_lane_point_values));
    MEMSET(mid_band_lane_point_values, 0, sizeof(mid_band_lane_point_values));
    MEMSET(high_band_lane_point_values, 0, sizeof(high_band_lane_point_values));
    MEMSET(mid_band_lane_chroma_id, 0, sizeof(mid_band_lane_chroma_id));
    MEMSET(mid_band_chroma_mask_field, 0, sizeof(mid_band_chroma_mask_field));
    MEMSET(high_band_spectral_flatness_glitter_field, 0, sizeof(high_band_spectral_flatness_glitter_field));
    fft_data.history_frame_pos = 0;
    fft_data.frame_pos = 0;
    onset_gate = 0.0f;
    adaptive_onset_mean = 0.0f;
    adaptive_onset_deviation = 0.0f;
    spectral_flatness_glitter_alpha = 0.0f;
    adaptive_glitter_power_mean = 0.0f;
    adaptive_glitter_power_deviation = 0.0f;
    adaptive_glitter_power_ready = 0;
    adaptive_chroma_mask_mean = 0.0f;
    adaptive_chroma_mask_deviation = 0.0f;
    adaptive_chroma_mask_ready = 0;
    low_band_adaptive_peak = 0.0f;
    mid_band_adaptive_peak = 0.0f;
    high_band_adaptive_peak = 0.0f;

    for (int i = ANALYSIS_FFT_HISTORY_FRAME_COUNT - 1; i >= 0; i--) {
        int replay_frame_offset = WRAP(i * AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES, wave.frameCount);
        int chunk_start_frame = WRAP_MINUS(wave_cursor, replay_frame_offset, wave.frameCount);
        fffftt_inspection_fill_analysis_window(chunk_start_frame);
        apply_blackman_window();
        shz_fft((shz_complex_t*)fft_data.work_buffer, (size_t)ANALYSIS_WINDOW_SIZE_IN_FRAMES);
        build_spectrum();
        consume_current_fft_frame();
    }

    inspection_ready = 1;
    retention_window_max_frame_pos = fft_data.frame_pos - 1;
    retention_window_min_frame_pos = retention_window_max_frame_pos - (ANALYSIS_FFT_HISTORY_FRAME_COUNT - 1);
    cur_frame_pos = retention_window_max_frame_pos;
    int cur_history_frame_pos = WRAP(cur_frame_pos, ANALYSIS_FFT_HISTORY_FRAME_COUNT);
    onset_gate = onset_gate_history[cur_history_frame_pos];
    spectral_flatness_glitter_alpha = spectral_flatness_glitter_alpha_history[cur_history_frame_pos];
    for (int i = 0; i < HIGH_BAND_VERTEX_COUNT; i++) {
        high_band_spectral_flatness_glitter_field[i] = spectral_flatness_glitter_alpha;
    }
}
