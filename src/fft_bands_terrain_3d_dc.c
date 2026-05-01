#define FFFFTT_PROFILE_FFT_TERRAIN_3D
#include "fffftt.h"
#include <GL/gl.h>

#define LINE_WIDTH_RASTER_PIXELS 1.0f
#define POINT_SIZE_RASTER_PIXELS 3.0f

#define LOW_BAND_POINT_COUNT 9
#define MID_BAND_POINT_COUNT LANE_POINT_COUNT
#define HIGH_BAND_POINT_COUNT 49
#define WHITE_NOISE_TEXELS_PER_QUAD 16
//TODO: look up tables to make it clear what the bands actually consist of,
//NOTE: THESE ARE MANUALLY WRITTEN AND SHOULD STAY IN ALIGNMENT WITH ANY CHANGES TO THE ABOVE *_BAND_POINT_COUNT values!
static const unsigned short LOW_BAND_BIN_BOUNDS[LOW_BAND_POINT_COUNT][2] = {
    // 20 Hz -> 400 Hz : bins 1..19
    {1, 2},
    {2, 4},
    {4, 6},
    {6, 9},
    {8, 11},
    {11, 13},
    {13, 15},
    {15, 18},
    {17, 19},
};

static const unsigned short MID_BAND_BIN_BOUNDS[MID_BAND_POINT_COUNT][2] = {
    // 400 Hz -> 4000 Hz : bins 20..186
    {20, 23},   {22, 28},   {27, 33},   {32, 38},   {37, 43},   {42, 48},   {48, 54},   {53, 59},   {58, 64},   {63, 69},   {68, 74},
    {74, 80},   {79, 85},   {84, 90},   {89, 95},   {94, 100},  {100, 106}, {105, 111}, {110, 116}, {115, 121}, {120, 126}, {125, 131},
    {131, 137}, {136, 142}, {141, 147}, {146, 152}, {151, 157}, {157, 163}, {162, 168}, {167, 173}, {172, 178}, {177, 183}, {183, 186},
};

static const unsigned short HIGH_BAND_BIN_BOUNDS[HIGH_BAND_POINT_COUNT][2] = {
    // 4000 Hz -> Nyquist : bins 187..511
    {187, 190}, {190, 197}, {196, 204}, {203, 211}, {210, 217}, {217, 224}, {223, 231}, {230, 238}, {237, 244}, {244, 251}, {250, 258}, {257, 265}, {264, 271},
    {271, 278}, {277, 285}, {284, 292}, {291, 298}, {298, 305}, {304, 312}, {311, 319}, {318, 325}, {325, 332}, {331, 339}, {338, 346}, {345, 352}, {352, 359},
    {358, 366}, {365, 373}, {372, 379}, {379, 386}, {385, 393}, {392, 400}, {399, 406}, {406, 413}, {412, 420}, {419, 427}, {426, 433}, {433, 440}, {439, 447},
    {446, 454}, {453, 460}, {460, 467}, {466, 474}, {473, 481}, {480, 487}, {487, 494}, {493, 501}, {500, 508}, {507, 511},
};

#define LOW_BAND_VERTEX_COUNT (LANE_COUNT * LOW_BAND_POINT_COUNT)
#define MID_BAND_VERTEX_COUNT (LANE_COUNT * MID_BAND_POINT_COUNT)
#define HIGH_BAND_VERTEX_COUNT (LANE_COUNT * HIGH_BAND_POINT_COUNT)

static const char* domain = "FFT-BANDS-TERRAIN-3D-DC";

static Mesh low_band_mesh = {0};
static Mesh mid_band_mesh = {0};
static Mesh high_band_mesh = {0};

static Model low_band_model = {0};
static Model mid_band_model = {0};
static Model high_band_model = {0};

static float low_band_lane_point_values[LANE_COUNT][LOW_BAND_POINT_COUNT] = {0};
static float mid_band_lane_point_values[LANE_COUNT][MID_BAND_POINT_COUNT] = {0};
static float high_band_lane_point_values[LANE_COUNT][HIGH_BAND_POINT_COUNT] = {0};

static float low_band_vertices[LOW_BAND_VERTEX_COUNT * 3] = {0};
static float low_band_normals[LOW_BAND_VERTEX_COUNT * 3] = {0};
static Color low_band_colors[LOW_BAND_VERTEX_COUNT] = {0};

static float mid_band_vertices[MID_BAND_VERTEX_COUNT * 3] = {0};
static float mid_band_normals[MID_BAND_VERTEX_COUNT * 3] = {0};
static Color mid_band_colors[MID_BAND_VERTEX_COUNT] = {0};
static float mid_band_texcoords[MID_BAND_VERTEX_COUNT * 2] = {0};
static unsigned char mid_chroma_index_field[LANE_COUNT][MID_BAND_POINT_COUNT] = {0};
static float mid_chroma_strength_field[LANE_COUNT][MID_BAND_POINT_COUNT] = {0};

static float high_band_vertices[HIGH_BAND_VERTEX_COUNT * 3] = {0};
static float high_band_normals[HIGH_BAND_VERTEX_COUNT * 3] = {0};
static Color high_band_colors[HIGH_BAND_VERTEX_COUNT] = {0};
static float high_band_texcoords[HIGH_BAND_VERTEX_COUNT * 2] = {0};
static float high_band_timed_glitter_field[LANE_COUNT][HIGH_BAND_POINT_COUNT] = {0};
static float high_band_spectral_flatness_glitter_field[HIGH_BAND_VERTEX_COUNT] = {0};
static float spectral_flatness_glitter_scale = 0.0f;
static float spectral_flatness_glitter_scale_history[ANALYSIS_FFT_HISTORY_FRAME_COUNT] = {0};

static void build_bands_terrain(int lane, const float* bin_levels);
static void build_band_terrain(float* front_lane, int point_count, const float* bin_levels, const unsigned short band_bin_bounds[][2]);
static void build_timed_glitter_color_field(int lane, const float* bin_levels);
static void update_mesh_colors_timed_glitter(Color* colors, const float* glitter_field, int point_count, float time);
static void update_spectral_flatness_glitter_scale(const float* raw_spectrum);
static void update_mesh_colors_spectral_flatness_glitter(Color* colors, const float* glitter_field);

static int inspection_ready = 0;
static int inspection_frame = 0;
static int inspection_frame_oldest = 0;
static int inspection_frame_newest = 0;
static void update_playback_controls_fft(void);
static void consume_latest_fft_history_frame(void);
static void build_fft_terrain_lane_from_history(int lane, int frame);
static void inspection_step(int dir);
static void rebuild_fft_terrain_meshes(void);
static void rebase_fft_history(void);

int main(void) {
    // SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    font = LoadFont(RD_FONT);
    fft_data.tapback_pos = ANALYSIS_TAPBACK_POS_DEFAULT;
    fft_data.work_buffer = RL_CALLOC(ANALYSIS_WINDOW_SIZE_IN_FRAMES, sizeof(FFTComplex));
    fft_data.prev_spectrum_bin_levels = RL_CALLOC(ANALYSIS_SPECTRUM_BIN_COUNT, sizeof(float));
    fft_data.raw_spectrum_history_levels = RL_CALLOC(ANALYSIS_FFT_HISTORY_FRAME_COUNT, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]));
    fft_data.spectrum_history_levels = RL_CALLOC(ANALYSIS_FFT_HISTORY_FRAME_COUNT, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]));

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
    load_audio_tracks();
    set_audio_track(DEFAULT_AUDIO_TRACK_SHADERTOY_EXPERIMENT);
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
    low_band_model = LoadModelFromMesh(low_band_mesh);
    unsigned char* low_saved_colors = low_band_model.meshes[0].colors;

    mid_band_mesh = GenMeshPlane(1.0f, 1.0f, MID_BAND_POINT_COUNT - 1, LANE_COUNT - 1);
    mid_band_mesh.colors = RL_CALLOC(mid_band_mesh.vertexCount, sizeof(Color));
    Texture2D lane_mask_texture = build_lane_mask(mid_band_texcoords, MID_BAND_POINT_COUNT);
    wave_cursor_texture = build_lane_mask_glow(mid_band_texcoords, MID_BAND_POINT_COUNT);
    mid_band_model = LoadModelFromMesh(mid_band_mesh);
    unsigned char* mid_saved_colors = mid_band_model.meshes[0].colors;
    mid_band_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = lane_mask_texture;
    mid_band_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = 0;

    high_band_mesh = GenMeshPlane(1.0f, 1.0f, HIGH_BAND_POINT_COUNT - 1, LANE_COUNT - 1);
    high_band_mesh.colors = RL_CALLOC(high_band_mesh.vertexCount, sizeof(Color));
    int* white_noise_dimensions = WHITE_NOISE_MASK_DIMS(HIGH_BAND_POINT_COUNT, WHITE_NOISE_TEXELS_PER_QUAD);
    Texture2D white_noise_texture =
        build_white_noise_mask(white_noise_dimensions[0], white_noise_dimensions[1], high_band_texcoords, HIGH_BAND_POINT_COUNT, WHITE_NOISE_TEXELS_PER_QUAD);
    high_band_model = LoadModelFromMesh(high_band_mesh);
    unsigned char* high_saved_colors = high_band_model.meshes[0].colors;
    Color spectral_flatness_glitter_tint = WHITE;
    high_band_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = white_noise_texture;
    int white_noise_texture_id = high_band_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id;
    high_band_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = 0;

    wave_cursor_model = &mid_band_model;

    fill_mesh_colors(low_band_colors, LOW_BAND_POINT_COUNT);
    fill_mesh_colors(mid_band_colors, MID_BAND_POINT_COUNT);
    fill_mesh_colors(high_band_colors, HIGH_BAND_POINT_COUNT);
    // update_mesh_colors_timed_glitter(high_band_colors, &high_band_timed_glitter_field[0][0], HIGH_BAND_POINT_COUNT, 0.0f);
    // update_mesh_colors_spectral_flatness_glitter(high_band_colors, high_band_spectral_flatness_glitter_field);

    update_mesh_vertices(low_band_vertices, &low_band_lane_point_values[0][0], LOW_BAND_POINT_COUNT);
    update_mesh_normals_smooth(low_band_normals, low_band_vertices, LOW_BAND_POINT_COUNT);
    build_mesh_smooth(&low_band_mesh, low_band_vertices, low_band_normals, low_band_colors, low_band_mesh.texcoords, LOW_BAND_VERTEX_COUNT);

    update_mesh_vertices(mid_band_vertices, &mid_band_lane_point_values[0][0], MID_BAND_POINT_COUNT);
    update_mesh_normals_smooth(mid_band_normals, mid_band_vertices, MID_BAND_POINT_COUNT);
    update_mesh_colors_pitch_class(mid_band_colors, &mid_chroma_index_field[0][0], &mid_chroma_strength_field[0][0], MID_BAND_POINT_COUNT);
    build_mesh_smooth(&mid_band_mesh, mid_band_vertices, mid_band_normals, mid_band_colors, mid_band_texcoords, MID_BAND_VERTEX_COUNT);

    update_mesh_vertices(high_band_vertices, &high_band_lane_point_values[0][0], HIGH_BAND_POINT_COUNT);
    update_mesh_normals_smooth(high_band_normals, high_band_vertices, HIGH_BAND_POINT_COUNT);
    build_mesh_smooth(&high_band_mesh, high_band_vertices, high_band_normals, high_band_colors, high_band_texcoords, HIGH_BAND_VERTEX_COUNT);
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        update_playback_controls_fft();

        int audio_dirty = 0;
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

            apply_blackman_window();
            shz_fft((shz_complex_t*)fft_data.work_buffer, (size_t)ANALYSIS_WINDOW_SIZE_IN_FRAMES);
            build_spectrum();
            consume_latest_fft_history_frame();
            audio_dirty = 1;
        }

        if (audio_dirty) {
            rebuild_fft_terrain_meshes();
        }

        update_audio_track_cycle();
        update_mesh_texcoords_smooth_scroll(white_noise_dimensions[0],
                                            white_noise_dimensions[1],
                                            high_band_mesh.texcoords,
                                            HIGH_BAND_POINT_COUNT,
                                            WHITE_NOISE_TEXELS_PER_QUAD,
                                            (float)GetTime());
        update_camera_orbit(&camera, GetFrameTime());
        update_padmouse(GetFrameTime(), &camera);
        update_diffuse_strength();

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
        glLightfv(GL_LIGHT0, GL_POSITION, (const GLfloat[]){light0_position.x, light0_position.y, light0_position.z, 1.0f});
        DrawModelEx(low_band_model, BOTTOM_BASS, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);
        glDisable(GL_LIGHTING);

        DrawModelEx(mid_band_model, MIDDLE, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);
        low_band_model.meshes[0].colors = NULL;
        mid_band_model.meshes[0].colors = NULL;
        high_band_model.meshes[0].colors = NULL;
        high_band_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = white_noise_texture_id;
        // DrawModelEx(high_band_model, TOP, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);
        spectral_flatness_glitter_tint.a = (unsigned char)(spectral_flatness_glitter_scale * (float)DRAW_COLOR_CHANNEL_MAX);
        DrawModelPointsEx(high_band_model, TOP, Y_AXIS, 0.0f, DEFAULT_SCALE, spectral_flatness_glitter_tint);
        high_band_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = 0;
        low_band_model.meshes[0].colors = low_saved_colors;
        mid_band_model.meshes[0].colors = mid_saved_colors;
        high_band_model.meshes[0].colors = high_saved_colors;

        if (is_paused) {
            draw_paused_wave_cursor_lane_marker(); // TODO: make this actually idempotentent
            mid_band_model.meshes[0].colors = mid_saved_colors;
        }
        draw_light_position_marker(light0_position);

        EndMode3D();
        DrawTextEx(font, TextFormat("%2i FPS", GetFPS()), (Vector2){50.0f, 440.0f}, FONT_SIZE, 0.0f, WHITE);
        draw_playback_inspection_hud();
        draw_padmouse();
        EndDrawing();
    }

    UnloadTexture(lane_mask_texture);
    UnloadTexture(wave_cursor_texture);
    UnloadTexture(white_noise_texture);
    UnloadModel(low_band_model);
    UnloadModel(mid_band_model);
    UnloadModel(high_band_model);
    UnloadAudioStream(audio_stream);
    unload_audio_tracks();
    CloseAudioDevice();
    RL_FREE(fft_data.raw_spectrum_history_levels);
    RL_FREE(fft_data.spectrum_history_levels);
    RL_FREE(fft_data.prev_spectrum_bin_levels);
    RL_FREE(fft_data.work_buffer);
    UnloadFont(font);
    CloseWindow();
    return 0;
}

static void build_bands_terrain(int lane, const float* bin_levels) {
    build_band_terrain(&low_band_lane_point_values[lane][0], LOW_BAND_POINT_COUNT, bin_levels, LOW_BAND_BIN_BOUNDS);
    build_band_terrain(&mid_band_lane_point_values[lane][0], MID_BAND_POINT_COUNT, bin_levels, MID_BAND_BIN_BOUNDS);
    build_band_terrain(&high_band_lane_point_values[lane][0], HIGH_BAND_POINT_COUNT, bin_levels, HIGH_BAND_BIN_BOUNDS);
    // TODO: THIS NEEDS ADDRESSED because i still want *_BAND_BIN_BOUNDS pattern for explicitness but `build_pitch_class_color_field` will
    // end up doing the whole generalized `spectrum_band_point_sample_bin_bounds` rederivation of the tables, needs confidence on a single common pattern...
    build_pitch_class_color_field(&mid_chroma_index_field[lane][0],
                                  &mid_chroma_strength_field[lane][0],
                                  &mid_band_lane_point_values[lane][0],
                                  bin_levels,
                                  MID_BAND_POINT_COUNT,
                                  MID_BAND_BIN_BOUNDS[0][0],
                                  MID_BAND_BIN_BOUNDS[MID_BAND_POINT_COUNT - 1][1]);
    build_timed_glitter_color_field(lane, bin_levels);
}

static void build_band_terrain(float* front_lane, int point_count, const float* bin_levels, const unsigned short band_bin_bounds[][2]) {
    for (int i = 0; i < point_count; i++) {
        int bin_min = band_bin_bounds[i][0];
        int bin_max = band_bin_bounds[i][1];
        float level_sum = 0.0f;
        float peak_level = 0.0f;

        for (int j = bin_min; j <= bin_max; j++) {
            level_sum += bin_levels[j];
            peak_level = FMAXF(peak_level, bin_levels[j]);
        }

        float avg_level = level_sum / (float)(bin_max - bin_min + 1);
        front_lane[i] = POWF(CLAMP(avg_level * 0.80f + peak_level * 0.20f, 0.0f, 1.0f), 0.85f);
    }
}

static void build_timed_glitter_color_field(int lane, const float* bin_levels) {
    float lane_glitter_seeds[HIGH_BAND_POINT_COUNT] = {0};
    const float* lag_spectrum = bin_levels;
    if (inspection_ready) {
        int frame = inspection_frame - lane;
        int lag_frame = frame - ONSET_LAG_FRAMES;
        if (lag_frame >= inspection_frame_oldest && lag_frame <= inspection_frame_newest) {
            int lag_index = lag_frame % ANALYSIS_FFT_HISTORY_FRAME_COUNT;
            lag_spectrum = fft_data.spectrum_history_levels[lag_index];
        }
    } else if (fft_data.frame_index > ONSET_LAG_FRAMES) {
        int lag_index = (fft_data.history_pos - 1 - ONSET_LAG_FRAMES + ANALYSIS_FFT_HISTORY_FRAME_COUNT) % ANALYSIS_FFT_HISTORY_FRAME_COUNT;
        lag_spectrum = fft_data.spectrum_history_levels[lag_index];
    }

    for (int i = 0; i < HIGH_BAND_POINT_COUNT; i++) {
        const int bin_min = HIGH_BAND_BIN_BOUNDS[i][0];
        const int bin_max = HIGH_BAND_BIN_BOUNDS[i][1];
        const int bin_count = bin_max - bin_min + 1;
        float flux_sum = 0.0f;

        for (int j = bin_min; j <= bin_max; j++) {
            float reference_level = lag_spectrum[j];
            for (int k = -ONSET_ENVELOPE_RADIUS; k <= ONSET_ENVELOPE_RADIUS; k++) {
                int lag_bin = (j + k < 0) ? 0 : ((j + k > ANALYSIS_SPECTRUM_BIN_COUNT - 1) ? ANALYSIS_SPECTRUM_BIN_COUNT - 1 : j + k);
                reference_level = FMAXF(reference_level, lag_spectrum[lag_bin]);
            }
            flux_sum += FMAXF(bin_levels[j] - reference_level, 0.0f);
        }

        const int prev_i = (i > 0) ? i - 1 : i;
        const int next_i = (i < HIGH_BAND_POINT_COUNT - 1) ? i + 1 : i;
        const float prev = high_band_lane_point_values[lane][prev_i];
        const float next = high_band_lane_point_values[lane][next_i];
        const float ridge = high_band_lane_point_values[lane][i];
        const float avg_flux = flux_sum / (float)bin_count;
        const float flux = CLAMP((avg_flux - 0.0025f) / 0.0375f, 0.0f, 1.0f);
        const float curvy = CLAMP(FABSF(prev - 2.0f * ridge + next) / 0.125f, 0.0f, 1.0f);
        const float peaky = CLAMP((ridge - 0.5f * (prev + next)) / 0.10f, 0.0f, 1.0f);
        const float glitter_seed =
            ridge * (0.25f + flux * flux * (2.25f - 1.5f * flux)) + curvy * curvy * (0.60f - 0.40f * curvy) + peaky * peaky * (0.45f - 0.30f * peaky);
        lane_glitter_seeds[i] = POWF(CLAMP(glitter_seed, 0.0f, 1.0f), 0.75f);
    }

    for (int i = 0; i < HIGH_BAND_POINT_COUNT; i++) {
        const float center = lane_glitter_seeds[i];
        const float left = (i > 0) ? lane_glitter_seeds[i - 1] * 0.60f : 0.0f;
        const float right = (i < HIGH_BAND_POINT_COUNT - 1) ? lane_glitter_seeds[i + 1] * 0.60f : 0.0f;
        const float decay = (lane + 1 < LANE_COUNT) ? high_band_timed_glitter_field[lane + 1][i] * 0.84f : 0.0f;
        high_band_timed_glitter_field[lane][i] = FMAXF(FMAXF(center, left), FMAXF(right, decay));
    }
}

static void update_mesh_colors_timed_glitter(Color* colors, const float* glitter_field, int point_count, float time) {
    for (int i = 0; i < LANE_COUNT; i++) {
        for (int j = 0; j < point_count; j++) {
            const float glitter = glitter_field[i * point_count + j];
            const float phase = time * 10.5f + j * 0.85f + i * 1.15f + glitter * 11.0f;
            const float wave = 0.5f + 0.5f * SINF(phase);
            const float shimmer = 0.243f + 1.107f * POWF(wave, 3.5f);
            const float alpha = CLAMP((glitter * shimmer - 0.05f) / 0.95f, 0.0f, 1.0f);
            colors[i * point_count + j] = WHITE;
            colors[i * point_count + j].a = (unsigned char)(alpha * (float)DRAW_COLOR_CHANNEL_MAX);
        }
    }
}

// https://librosa.org/doc/main/_modules/librosa/feature/spectral.html#spectral_flatness
#define SPECTRAL_FLATNESS_AMIN 1.0e-10f // amin: float = 1e-10,
#define SPECTRAL_FLATNESS_POWER 2.0f    // power: float = 2.0,

//TODO: TUNE THIS BUT ONLY IF ITS USEFUL AFTER LIGHTING STUFF...
// waste of time to focus on the per-wav envelopes. it jitters or is completely transparent for shadertoy tracks
// three ideas:
// 1. somehow pre-process the wav data to sort of "normalize" the assets into a common envelope
// 2. find more librosa application examples that handle generic/sensitive wav envelope varieties??
// 3. smoothing of the attack and release ramp needs work
#define SPECTRAL_FLATNESS_GLITTER_SILENCE_GATE_POWER_LOW 1.0e-9f  //1.0e-7f
#define SPECTRAL_FLATNESS_GLITTER_SILENCE_GATE_POWER_HIGH 1.0e-9f //1.0e-7f

#define SPECTRAL_FLATNESS_GLITTER_ATTACK_RATE 1.0f  //0.95f
#define SPECTRAL_FLATNESS_GLITTER_RELEASE_RATE 1.0f //1.0f

static void update_spectral_flatness_glitter_scale(const float* raw_spectrum) {
    // https://librosa.org/doc/main/generated/librosa.feature.spectral_flatness.html
    const int bin_min = HIGH_BAND_BIN_BOUNDS[0][0];
    const int bin_max = HIGH_BAND_BIN_BOUNDS[HIGH_BAND_POINT_COUNT - 1][1];
    const int bin_count = bin_max - bin_min + 1;
    float log_power_sum = 0.0f;
    float arithmetic_power_sum = 0.0f;

    for (int i = bin_min; i <= bin_max; i++) {
        const float power = FMAXF(POWF(raw_spectrum[i], SPECTRAL_FLATNESS_POWER), SPECTRAL_FLATNESS_AMIN);
        log_power_sum += LOGF(power);
        arithmetic_power_sum += power;
    }
    const float geometric_mean = EXPF(log_power_sum / (float)bin_count);
    const float arithmetic_mean = arithmetic_power_sum / (float)bin_count;
    const float spectral_flatness = geometric_mean / FMAXF(arithmetic_mean, SPECTRAL_FLATNESS_AMIN);
    float scalar = CLAMP(spectral_flatness, 0.0f, 1.0f);
    float presence = 1.0f;
    if (SPECTRAL_FLATNESS_GLITTER_SILENCE_GATE_POWER_HIGH > 0.0f) {
        const float gate_low = SPECTRAL_FLATNESS_GLITTER_SILENCE_GATE_POWER_LOW;
        const float gate_high = SPECTRAL_FLATNESS_GLITTER_SILENCE_GATE_POWER_HIGH;
        presence = CLAMP((arithmetic_mean - gate_low) / (gate_high - gate_low), 0.0f, 1.0f);
        presence = presence * presence * (3.0f - 2.0f * presence); //TODO: smoothstep not a fan of this one...
    }
    const float scalar_target = scalar * presence;
    const float smoothing_rate =
        (scalar_target > spectral_flatness_glitter_scale) ? SPECTRAL_FLATNESS_GLITTER_ATTACK_RATE : SPECTRAL_FLATNESS_GLITTER_RELEASE_RATE;
    spectral_flatness_glitter_scale = LERP(spectral_flatness_glitter_scale, scalar_target, smoothing_rate);
    spectral_flatness_glitter_scale = CLAMP(spectral_flatness_glitter_scale, 0.0f, 1.0f);
    int current_index = (fft_data.history_pos - 1 + ANALYSIS_FFT_HISTORY_FRAME_COUNT) % ANALYSIS_FFT_HISTORY_FRAME_COUNT;
    spectral_flatness_glitter_scale_history[current_index] = spectral_flatness_glitter_scale;
    //TODO: and then do this for anticipating a distributed field from a 1D signal measurement... later...
    for (int i = 0; i < HIGH_BAND_VERTEX_COUNT; i++) {
        high_band_spectral_flatness_glitter_field[i] = spectral_flatness_glitter_scale;
    }
}

static void update_mesh_colors_spectral_flatness_glitter(Color* colors, const float* glitter_field) {
    for (int i = 0; i < HIGH_BAND_VERTEX_COUNT; i++) {
        colors[i] = WHITE;
        colors[i].a = (unsigned char)(glitter_field[i] * (float)DRAW_COLOR_CHANNEL_MAX);
    }
}

static void consume_latest_fft_history_frame(void) {
    int history_index = (fft_data.history_pos - 1 + ANALYSIS_FFT_HISTORY_FRAME_COUNT) % ANALYSIS_FFT_HISTORY_FRAME_COUNT;
    float* bin_levels = fft_data.spectrum_history_levels[history_index];
    advance_lane_history(&low_band_lane_point_values[0][0], LOW_BAND_POINT_COUNT);
    advance_lane_history(&mid_band_lane_point_values[0][0], MID_BAND_POINT_COUNT);
    advance_lane_history(&high_band_lane_point_values[0][0], HIGH_BAND_POINT_COUNT);
    advance_lane_history_u8(&mid_chroma_index_field[0][0], MID_BAND_POINT_COUNT);
    advance_lane_history(&mid_chroma_strength_field[0][0], MID_BAND_POINT_COUNT);
    advance_lane_history(&high_band_timed_glitter_field[0][0], HIGH_BAND_POINT_COUNT);
    build_bands_terrain(0, bin_levels);
    update_onset_interpolation_factor_fft(&fft_data);
    update_spectral_flatness_glitter_scale(fft_data.raw_spectrum_history_levels[history_index]);
}

static void update_playback_controls_fft(void) {
    int analysis_dirty = 0;

    if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
        reset_sticky_nav();
        if (!is_paused) {
            is_paused = true;
            wave_cursor = (wave_cursor + wave.frameCount - AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES) % wave.frameCount;
            paused_wave_cursor = wave_cursor;
            seek_delta_chunks = 0;
            for (int i = 0; i < MAX_DRAIN_CHUNK_COUNT; i++) {
                while (!IsAudioStreamProcessed(audio_stream)) {
                    /* KEEP DRAINING! */
                }
                UpdateAudioStream(audio_stream, drain_chunk_samples, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
            }
            PauseAudioStream(audio_stream);
            rebase_fft_history();
            rebuild_fft_terrain_meshes();
        } else {
            is_paused = false;
            inspection_ready = 0;
            PlayAudioStream(audio_stream);
            while (IsAudioStreamProcessed(audio_stream)) {
                for (int i = 0; i < AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES; i++) {
                    resume_chunk_samples[i] = wave_pcm16[wave_cursor];
                    if (++wave_cursor >= wave.frameCount) {
                        wave_cursor = 0;
                    }
                }
                UpdateAudioStream(audio_stream, resume_chunk_samples, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);

                for (int i = 0; i < ANALYSIS_WINDOW_SIZE_IN_FRAMES; i++) {
                    analysis_window_samples[i] = (float)resume_chunk_samples[i] / ANALYSIS_PCM16_UPPER_BOUND;
                }

                apply_blackman_window();
                shz_fft((shz_complex_t*)fft_data.work_buffer, (size_t)ANALYSIS_WINDOW_SIZE_IN_FRAMES);
                build_spectrum();
                consume_latest_fft_history_frame();
                analysis_dirty = 1;
            }
        }
    }

    if (is_paused && sticky_nav(GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
        wave_cursor = (wave_cursor + wave.frameCount - AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES) % wave.frameCount;
        seek_delta_chunks--;
        int target_frame = inspection_frame - 1;
        if (inspection_ready && inspection_frame > 0 && target_frame >= inspection_frame_oldest + (LANE_COUNT - 1)) {
            inspection_frame = target_frame;
            inspection_step(BACKWARD);
        } else {
            rebase_fft_history();
        }
        int history_index = (inspection_frame + ANALYSIS_FFT_HISTORY_FRAME_COUNT) % ANALYSIS_FFT_HISTORY_FRAME_COUNT;
        onset_interpolation_factor = onset_interpolation_factor_history[history_index];
        spectral_flatness_glitter_scale = spectral_flatness_glitter_scale_history[history_index];
        for (int i = 0; i < HIGH_BAND_VERTEX_COUNT; i++) {
            high_band_spectral_flatness_glitter_field[i] = spectral_flatness_glitter_scale;
        }
        rebuild_fft_terrain_meshes();
    } else if (is_paused && sticky_nav(GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
        wave_cursor = (wave_cursor + AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES) % wave.frameCount;
        seek_delta_chunks++;
        int target_frame = inspection_frame + 1;
        if (inspection_ready && target_frame <= inspection_frame_newest) {
            inspection_frame = target_frame;
            inspection_step(FORWARD);
        } else if (inspection_ready && inspection_frame == inspection_frame_newest) {
            for (int i = 0; i < ANALYSIS_WINDOW_SIZE_IN_FRAMES; i++) {
                int src = (wave_cursor + i) % wave.frameCount;
                analysis_window_samples[i] = (float)wave_pcm16[src] / ANALYSIS_PCM16_UPPER_BOUND;
            }

            apply_blackman_window();
            shz_fft((shz_complex_t*)fft_data.work_buffer, (size_t)ANALYSIS_WINDOW_SIZE_IN_FRAMES);
            build_spectrum();

            int frame = fft_data.frame_index - 1;
            update_onset_interpolation_factor_fft(&fft_data);
            int history_index = (fft_data.history_pos - 1 + ANALYSIS_FFT_HISTORY_FRAME_COUNT) % ANALYSIS_FFT_HISTORY_FRAME_COUNT;
            update_spectral_flatness_glitter_scale(fft_data.raw_spectrum_history_levels[history_index]);

            inspection_frame_newest = frame;
            if (inspection_frame_newest - inspection_frame_oldest >= ANALYSIS_FFT_HISTORY_FRAME_COUNT) {
                inspection_frame_oldest = inspection_frame_newest - (ANALYSIS_FFT_HISTORY_FRAME_COUNT - 1);
            }
            inspection_frame = inspection_frame_newest;
            inspection_step(FORWARD);
        } else {
            rebase_fft_history();
        }
        int history_index = (inspection_frame + ANALYSIS_FFT_HISTORY_FRAME_COUNT) % ANALYSIS_FFT_HISTORY_FRAME_COUNT;
        onset_interpolation_factor = onset_interpolation_factor_history[history_index];
        spectral_flatness_glitter_scale = spectral_flatness_glitter_scale_history[history_index];
        for (int i = 0; i < HIGH_BAND_VERTEX_COUNT; i++) {
            high_band_spectral_flatness_glitter_field[i] = spectral_flatness_glitter_scale;
        }
        rebuild_fft_terrain_meshes();
    }

    if (analysis_dirty) {
        rebuild_fft_terrain_meshes();
    }
}

static void build_fft_terrain_lane_from_history(int lane, int frame) {
    if (frame < inspection_frame_oldest || frame > inspection_frame_newest) {
        MEMSET(low_band_lane_point_values[lane], 0, sizeof(low_band_lane_point_values[lane]));
        MEMSET(mid_band_lane_point_values[lane], 0, sizeof(mid_band_lane_point_values[lane]));
        MEMSET(high_band_lane_point_values[lane], 0, sizeof(high_band_lane_point_values[lane]));
        MEMSET(mid_chroma_index_field[lane], 0, sizeof(mid_chroma_index_field[lane]));
        MEMSET(mid_chroma_strength_field[lane], 0, sizeof(mid_chroma_strength_field[lane]));
        MEMSET(high_band_timed_glitter_field[lane], 0, sizeof(high_band_timed_glitter_field[lane]));
        return;
    }

    int history_index = (int)(frame % ANALYSIS_FFT_HISTORY_FRAME_COUNT);
    float* bin_levels = fft_data.spectrum_history_levels[history_index];
    build_bands_terrain(lane, bin_levels);
}

static void inspection_step(int dir) {
    for (int i = (dir == FORWARD) ? LANE_COUNT - 1 : 0; i != ((dir == FORWARD) ? 0 : LANE_COUNT - 1); i -= dir) {
        MEMCPY(lane_point_values[i], lane_point_values[i - dir], sizeof(lane_point_values[i]));
        MEMCPY(low_band_lane_point_values[i], low_band_lane_point_values[i - dir], sizeof(low_band_lane_point_values[i]));
        MEMCPY(mid_band_lane_point_values[i], mid_band_lane_point_values[i - dir], sizeof(mid_band_lane_point_values[i]));
        MEMCPY(high_band_lane_point_values[i], high_band_lane_point_values[i - dir], sizeof(high_band_lane_point_values[i]));
        MEMCPY(mid_chroma_index_field[i], mid_chroma_index_field[i - dir], sizeof(mid_chroma_index_field[i]));
        MEMCPY(mid_chroma_strength_field[i], mid_chroma_strength_field[i - dir], sizeof(mid_chroma_strength_field[i]));
        MEMCPY(high_band_timed_glitter_field[i], high_band_timed_glitter_field[i - dir], sizeof(high_band_timed_glitter_field[i]));
    }
    build_fft_terrain_lane_from_history((dir == FORWARD) ? 0 : LANE_COUNT - 1, (dir == FORWARD) ? inspection_frame : inspection_frame - (LANE_COUNT - 1));
}

static void rebuild_fft_terrain_meshes(void) {
    update_mesh_vertices(low_band_vertices, &low_band_lane_point_values[0][0], LOW_BAND_POINT_COUNT);
    update_mesh_normals_smooth(low_band_normals, low_band_vertices, LOW_BAND_POINT_COUNT);
    build_mesh_smooth(&low_band_mesh, low_band_vertices, low_band_normals, low_band_colors, low_band_mesh.texcoords, LOW_BAND_VERTEX_COUNT);

    update_mesh_vertices(mid_band_vertices, &mid_band_lane_point_values[0][0], MID_BAND_POINT_COUNT);
    update_mesh_normals_smooth(mid_band_normals, mid_band_vertices, MID_BAND_POINT_COUNT);
    update_mesh_colors_pitch_class(mid_band_colors, &mid_chroma_index_field[0][0], &mid_chroma_strength_field[0][0], MID_BAND_POINT_COUNT);
    build_mesh_smooth(&mid_band_mesh, mid_band_vertices, mid_band_normals, mid_band_colors, mid_band_texcoords, MID_BAND_VERTEX_COUNT);

    update_mesh_vertices(high_band_vertices, &high_band_lane_point_values[0][0], HIGH_BAND_POINT_COUNT);
    update_mesh_normals_smooth(high_band_normals, high_band_vertices, HIGH_BAND_POINT_COUNT);
    update_mesh_colors_spectral_flatness_glitter(high_band_colors, high_band_spectral_flatness_glitter_field);
    // update_mesh_colors_timed_glitter(high_band_colors, &high_band_timed_glitter_field[0][0], HIGH_BAND_POINT_COUNT, (float)GetTime());
    build_mesh_smooth(&high_band_mesh, high_band_vertices, high_band_normals, high_band_colors, high_band_texcoords, HIGH_BAND_VERTEX_COUNT);
}

static void rebase_fft_history(void) {
    inspection_ready = 0;
    MEMSET(fft_data.prev_spectrum_bin_levels, 0, sizeof(float) * ANALYSIS_SPECTRUM_BIN_COUNT);
    MEMSET(fft_data.raw_spectrum_history_levels, 0, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]) * ANALYSIS_FFT_HISTORY_FRAME_COUNT);
    MEMSET(fft_data.spectrum_history_levels, 0, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]) * ANALYSIS_FFT_HISTORY_FRAME_COUNT);
    MEMSET(onset_interpolation_factor_history, 0, sizeof(onset_interpolation_factor_history));
    MEMSET(spectral_flatness_glitter_scale_history, 0, sizeof(spectral_flatness_glitter_scale_history));
    MEMSET(low_band_lane_point_values, 0, sizeof(low_band_lane_point_values));
    MEMSET(mid_band_lane_point_values, 0, sizeof(mid_band_lane_point_values));
    MEMSET(high_band_lane_point_values, 0, sizeof(high_band_lane_point_values));
    MEMSET(mid_chroma_index_field, 0, sizeof(mid_chroma_index_field));
    MEMSET(mid_chroma_strength_field, 0, sizeof(mid_chroma_strength_field));
    MEMSET(high_band_timed_glitter_field, 0, sizeof(high_band_timed_glitter_field));
    MEMSET(high_band_spectral_flatness_glitter_field, 0, sizeof(high_band_spectral_flatness_glitter_field));
    fft_data.history_pos = 0;
    fft_data.frame_index = 0;
    onset_interpolation_factor = 0.0f;
    spectral_flatness_glitter_scale = 0.0f;

    for (int i = ANALYSIS_FFT_HISTORY_FRAME_COUNT - 1; i >= 0; i--) {
        int replay_frame_offset = (i * AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES) % wave.frameCount;
        int chunk_start_frame = (wave_cursor + wave.frameCount - replay_frame_offset) % wave.frameCount;

        for (int j = 0; j < ANALYSIS_WINDOW_SIZE_IN_FRAMES; j++) {
            int src = (chunk_start_frame + j) % wave.frameCount;
            analysis_window_samples[j] = (float)wave_pcm16[src] / ANALYSIS_PCM16_UPPER_BOUND;
        }

        apply_blackman_window();
        shz_fft((shz_complex_t*)fft_data.work_buffer, (size_t)ANALYSIS_WINDOW_SIZE_IN_FRAMES);
        build_spectrum();
        consume_latest_fft_history_frame();
    }

    inspection_ready = 1;
    inspection_frame_newest = fft_data.frame_index - 1;
    inspection_frame_oldest = inspection_frame_newest - (ANALYSIS_FFT_HISTORY_FRAME_COUNT - 1);
    inspection_frame = inspection_frame_newest;
    int history_index = (inspection_frame + ANALYSIS_FFT_HISTORY_FRAME_COUNT) % ANALYSIS_FFT_HISTORY_FRAME_COUNT;
    onset_interpolation_factor = onset_interpolation_factor_history[history_index];
    spectral_flatness_glitter_scale = spectral_flatness_glitter_scale_history[history_index];
    for (int i = 0; i < HIGH_BAND_VERTEX_COUNT; i++) {
        high_band_spectral_flatness_glitter_field[i] = spectral_flatness_glitter_scale;
    }
}
