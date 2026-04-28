#define FFFFTT_PROFILE_WAVEFORM_TERRAIN_3D
#include "fffftt.h"
#include <GL/gl.h>

#define LINE_WIDTH_RASTER_PIXELS 1.0f
#define POINT_SIZE_RASTER_PIXELS 3.0f

static const char* domain = "WAVEFORM-TERRAIN-3D-DC";

static Mesh mesh_a = {0};
static Mesh mesh_b = {0};
static Mesh flat_mesh = {0};

static Model model_a = {0};
static Model model_b = {0};
static Model flat_model = {0};

static void init_hilbert_filter(void);
static void build_hilbert_normal_field(void);
static void update_mesh_normals_hilbert(float* normals, const float* src_normals, const float* normal_field, const float* vertices);
static void build_rms_color_field(void);
static void update_mesh_colors_rms(Color* colors);
static Color sample_rms_pallete(float rms);
static void update_onset_interpolation_factor_waveform(void);
static void update_playback_controls_waveform(void);

static float hilbert_normal_field[LANE_COUNT][LANE_POINT_COUNT] = {0};
static float rms_color_field[LANE_COUNT][LANE_POINT_COUNT] = {0};
static int hilbert_center_sample_indices[LANE_POINT_COUNT] = {0};
static float hilbert_filter_ideal[65] = {0};
static float hilbert_filter_window[65] = {0};

static float vertices[MESH_VERTEX_COUNT * 3] = {0};
static float normals[MESH_VERTEX_COUNT * 3] = {0};
static float hilbert_normals[MESH_VERTEX_COUNT * 3] = {0};
static Color colors[MESH_VERTEX_COUNT] = {0};
static Color rms_colors[MESH_VERTEX_COUNT] = {0};

static float flat_vertices[FLAT_VERTEX_COUNT * 3] = {0};
static float flat_normals[FLAT_VERTEX_COUNT * 3] = {0};
static float flat_hilbert_normals[FLAT_VERTEX_COUNT * 3] = {0};
static Color flat_colors[FLAT_VERTEX_COUNT] = {0};

int main(void) {
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    font = LoadFont(RD_FONT);

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
    wave = LoadWave(RD_SHADERTOY_EXPERIMENT_22K_WAV);
    // wave = LoadWave(RD_SHADERTOY_ELECTRONEBULAE_ONE_FOURTH_22K_WAV);
    // wave = LoadWave(RD_DDS_FFM_22K_WAV);
    // wave = LoadWave(RD_RAMA_22K_WAV);
    WaveFormat(&wave, SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    audio_stream = LoadAudioStream(SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    PlayAudioStream(audio_stream);

    wave_pcm16 = (int16_t*)wave.data;

    Camera3D camera = {
        .position = (Vector3){1.456f * 2.0, 1.345f * 2.0, -1.366f * 2.0},
        .target = (Vector3){0.0f, 0.25f, 0.0f},
        .up = Y_AXIS,
        .fovy = 6.66f, //NOTE: satan
        // .fovy = 3.111f,
        .projection = CAMERA_ORTHOGRAPHIC,
    };

    mesh_a = GenMeshPlane(1.0f, 1.0f, LANE_POINT_COUNT - 1, LANE_COUNT - 1);
    mesh_a.colors = RL_CALLOC(mesh_a.vertexCount, sizeof(Color));
    Texture2D lane_mask_texture = build_lane_mask(mesh_a.texcoords, LANE_POINT_COUNT);
    wave_cursor_texture = build_lane_mask_glow(mesh_a.texcoords, LANE_POINT_COUNT);

    model_a = LoadModelFromMesh(mesh_a);
    unsigned char* saved_colors = model_a.meshes[0].colors;
    model_a.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = lane_mask_texture;
    int saved_texture_id = model_a.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id;
    model_a.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = 0;
    wave_cursor_model = &model_a;

    mesh_b = GenMeshPlane(1.0f, 1.0f, LANE_POINT_COUNT - 1, LANE_COUNT - 1);
    mesh_b.colors = RL_CALLOC(mesh_b.vertexCount, sizeof(Color));
    model_b = LoadModelFromMesh(mesh_b);

    flat_mesh = (Mesh){
        .vertexCount = mesh_a.triangleCount * 3,
        .triangleCount = mesh_a.triangleCount,
        .vertices = RL_CALLOC(mesh_a.triangleCount * 3 * 3, sizeof(float)),
        .normals = RL_CALLOC(mesh_a.triangleCount * 3 * 3, sizeof(float)),
        .colors = RL_CALLOC(mesh_a.triangleCount * 3, sizeof(Color)),
        .indices = NULL, //NOTE: explicit here for didactic
    };
    flat_model = LoadModelFromMesh(flat_mesh);

    fill_mesh_colors(colors, LANE_POINT_COUNT);
    expand_mesh_colors_flat(flat_colors, colors, mesh_a.indices);
    init_hilbert_filter();

    update_mesh_vertices(vertices, &lane_point_values[0][0], LANE_POINT_COUNT);
    update_mesh_normals_smooth(normals, vertices, LANE_POINT_COUNT);

    build_hilbert_normal_field();
    update_mesh_normals_hilbert(hilbert_normals, normals, &hilbert_normal_field[0][0], vertices);
    build_rms_color_field();
    update_mesh_colors_rms(rms_colors);

    update_mesh_vertices_flat(flat_vertices, vertices, mesh_a.indices);
    update_mesh_normals_flat(flat_normals, flat_vertices);
    expand_mesh_normals_flat(flat_hilbert_normals, hilbert_normals, mesh_a.indices);

    // build_mesh_smooth(&mesh_a, vertices, normals, colors);
    build_mesh_smooth(&mesh_a, vertices, normals, colors, MESH_VERTEX_COUNT);
    // build_mesh_smooth(&mesh_a, vertices, hilbert_normals, colors);

    // build_mesh_smooth(&mesh_b, vertices, normals, colors);
    build_mesh_smooth(&mesh_b, vertices, normals, rms_colors, MESH_VERTEX_COUNT);

    // build_mesh_flat(&flat_mesh, flat_vertices, flat_normals, flat_colors);
    // build_mesh_flat(&flat_mesh, flat_vertices, flat_hilbert_normals, flat_colors);
    build_mesh_flat(&flat_mesh, flat_vertices, flat_hilbert_normals, flat_colors);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        update_playback_controls_waveform();

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

            advance_lane_history(&lane_point_values[0][0], LANE_POINT_COUNT);
            advance_lane_history(&hilbert_normal_field[0][0], LANE_POINT_COUNT);
            advance_lane_history(&rms_color_field[0][0], LANE_POINT_COUNT);
            for (int i = 0; i < LANE_POINT_COUNT; i++) {
                lane_point_values[0][i] = analysis_window_samples[(i * (ANALYSIS_WINDOW_SIZE_IN_FRAMES - 1)) / (LANE_POINT_COUNT - 1)];
            }
            update_onset_interpolation_factor_waveform();
            build_hilbert_normal_field();
            build_rms_color_field();
            audio_dirty = 1;
        }

        if (audio_dirty) {
            update_mesh_vertices(vertices, &lane_point_values[0][0], LANE_POINT_COUNT);
            update_mesh_normals_smooth(normals, vertices, LANE_POINT_COUNT);
            update_mesh_normals_hilbert(hilbert_normals, normals, &hilbert_normal_field[0][0], vertices);
            update_mesh_colors_rms(rms_colors);
            update_mesh_vertices_flat(flat_vertices, vertices, mesh_a.indices);
            update_mesh_normals_flat(flat_normals, flat_vertices);
            expand_mesh_normals_flat(flat_hilbert_normals, hilbert_normals, mesh_a.indices);

            build_mesh_smooth(&mesh_a, vertices, normals, colors, MESH_VERTEX_COUNT);
            // build_mesh_smooth(&mesh_a, vertices, hilbert_normals, colors);

            // build_mesh_smooth(&mesh_b, vertices, normals, colors);
            build_mesh_smooth(&mesh_b, vertices, normals, rms_colors, MESH_VERTEX_COUNT);

            // build_mesh_flat(&flat_mesh, flat_vertices, flat_normals, flat_colors);
            build_mesh_flat(&flat_mesh, flat_vertices, flat_hilbert_normals, flat_colors);
        }

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
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (const GLfloat[]){0.2f, 0.2f, 0.2f, 1.0f});
        glLightfv(GL_LIGHT0, GL_AMBIENT, (const GLfloat[]){0.0f, 0.0f, 0.0f, 1.0f});
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
        glLightfv(GL_LIGHT0, GL_POSITION, (const GLfloat[]){light0_position.x, light0_position.y, light0_position.z, 1.0f});
        DrawModelEx(model_a, MIDDLE, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);
        glDisable(GL_LIGHTING);
        // draw_light_position_marker();

        DrawModelEx(model_b, BOTTOM, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);

        model_a.meshes[0].colors = NULL;
        // model_a.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = saved_texture_id;
        // rlDisableDepthTest();
        // rlDisableBackfaceCulling();
        // DrawModelEx(model_a, MIDDLE, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);
        // rlEnableBackfaceCulling();
        // rlEnableDepthTest();
        // model_a.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = 0;

        glEnable(GL_LIGHTING);
        glShadeModel(GL_FLAT);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (const GLfloat[]){0.0f, 0.0f, 0.0f, 1.0f});
        glLightfv(GL_LIGHT0, GL_AMBIENT, (const GLfloat[]){0.0f, 0.0f, 0.0f, 1.0f});
        glLightfv(GL_LIGHT0, GL_DIFFUSE, (const GLfloat[]){1.0f, 1.0f, 1.0f, 1.0f});
        DrawModelEx(flat_model, TOP, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);
        glShadeModel(GL_SMOOTH);
        glDisable(GL_LIGHTING);

        DrawModelPointsEx(model_a, TOP, Y_AXIS, 0.0f, DEFAULT_SCALE, BLUE);
        DrawModelWiresEx(model_a, BOTTOM, Y_AXIS, 0.0f, DEFAULT_SCALE, MAGENTA);
        if (is_paused) {
            draw_paused_wave_cursor_lane_marker();
        }

        model_a.meshes[0].colors = saved_colors;

        EndMode3D();
        DrawTextEx(font, TextFormat("%2i FPS", GetFPS()), (Vector2){50.0f, 440.0f}, FONT_SIZE, 0.0f, WHITE);
        draw_playback_inspection_hud();
        // draw_padmouse();
        EndDrawing();
    }

    UnloadTexture(lane_mask_texture);
    UnloadTexture(wave_cursor_texture);
    UnloadModel(model_a);
    UnloadModel(model_b);
    UnloadModel(flat_model);
    UnloadAudioStream(audio_stream);
    UnloadWave(wave);
    CloseAudioDevice();
    UnloadFont(font);
    CloseWindow();
    return 0;
}

static void init_hilbert_filter(void) {
    const int hilbert_filter_length = 65;
    const int hilbert_filter_radius = hilbert_filter_length / 2;
    const float kaiser_beta = 8.0f;

    for (int i = 0; i < LANE_POINT_COUNT; i++) {
        hilbert_center_sample_indices[i] = (i * (ANALYSIS_WINDOW_SIZE_IN_FRAMES - 1)) / (LANE_POINT_COUNT - 1);
    }

    float kaiser_beta_i0 = 1.0f;
    float kaiser_term = 1.0f;
    for (int i = 1; i <= 12; i++) {
        float series_index = (float)i;
        kaiser_term *= (kaiser_beta * kaiser_beta) / (4.0f * series_index * series_index);
        kaiser_beta_i0 += kaiser_term;
    }

    for (int j = -hilbert_filter_radius; j <= hilbert_filter_radius; j++) {
        if (j == 0 || (j % 2) == 0) {
            continue;
        }

        float normalized_filter_position = (float)j / (float)hilbert_filter_radius;
        float kaiser_argument = kaiser_beta * SQRTF(FMAXF(0.0f, 1.0f - normalized_filter_position * normalized_filter_position));
        float kaiser_window_i0 = 1.0f;
        float window_term = 1.0f;
        for (int l = 1; l <= 12; l++) {
            float series_index = (float)l;
            window_term *= (kaiser_argument * kaiser_argument) / (4.0f * series_index * series_index);
            kaiser_window_i0 += window_term;
        }

        float kaiser_window = kaiser_window_i0 / kaiser_beta_i0;
        float ideal_hilbert = 2.0f / (PI * (float)j);
        hilbert_filter_ideal[j + hilbert_filter_radius] = ideal_hilbert;
        hilbert_filter_window[j + hilbert_filter_radius] = kaiser_window;
    }
}

static void build_hilbert_normal_field(void) {
    //Analytic-envelope normal-field mask references:
    //https://www.mathworks.com/help/signal/ref/envelope.html
    // (see all the envelope(,,'analytic') envelopes, the idea is to get the intersection of the envelope and the wavform for ridges...)
    //https://www.mathworks.com/help/signal/ref/hilbert.html
    // also
    const int hilbert_filter_radius = 65 / 2;
    const float contact_ratio_min = 0.72f;
    const float curvature_min = 0.012f;
    const float curvature_max = 0.075f;
    const float amplitude_min = 0.045f;
    const float amplitude_max = 0.20f;
    const float neighbor_contact_scale = 0.70f;

    float contact_mask_values[LANE_POINT_COUNT] = {0};
    float analysis_mean = 0.0f;

    for (int i = 0; i < ANALYSIS_WINDOW_SIZE_IN_FRAMES; i++) {
        analysis_mean += analysis_window_samples[i];
    }
    analysis_mean /= (float)ANALYSIS_WINDOW_SIZE_IN_FRAMES;

    for (int i = 0; i < LANE_POINT_COUNT; i++) {
        int center_sample_index = hilbert_center_sample_indices[i];
        float hilbert_sample = 0.0f;

        for (int j = -hilbert_filter_radius; j <= hilbert_filter_radius; j++) {
            float ideal_hilbert = hilbert_filter_ideal[j + hilbert_filter_radius];
            if (ideal_hilbert == 0.0f) {
                continue;
            }

            int k = center_sample_index - j;
            if (k < 0 || k >= ANALYSIS_WINDOW_SIZE_IN_FRAMES) {
                continue;
            }

            float centered_sample = analysis_window_samples[k] - analysis_mean;
            hilbert_sample += centered_sample * ideal_hilbert * hilbert_filter_window[j + hilbert_filter_radius];
        }

        float waveform_sample = lane_point_values[0][i];
        float real_sample = analysis_window_samples[center_sample_index] - analysis_mean;
        float local_analytic_envelope_value = SQRTF(real_sample * real_sample + hilbert_sample * hilbert_sample);
        float local_upper_envelope_value = analysis_mean + local_analytic_envelope_value;
        float local_lower_envelope_value = analysis_mean - local_analytic_envelope_value;
        float local_envelope_reference_value = (waveform_sample >= analysis_mean) ? local_upper_envelope_value : local_lower_envelope_value;

        float contact_ratio = 0.0f;
        if (local_analytic_envelope_value > 0.0f) {
            contact_ratio = 1.0f - (FABSF(waveform_sample - local_envelope_reference_value) / local_analytic_envelope_value);
        }
        contact_ratio = CLAMP((contact_ratio - contact_ratio_min) / (1.0f - contact_ratio_min), 0.0f, 1.0f);
        contact_ratio = contact_ratio * contact_ratio * (3.0f - 2.0f * contact_ratio);

        int i_prev = (i > 0) ? i - 1 : i;
        int i_next = (i < LANE_POINT_COUNT - 1) ? i + 1 : i;

        float waveform_sample_prev = lane_point_values[0][i_prev];
        float waveform_sample_next = lane_point_values[0][i_next];
        float local_curvature = FABSF(waveform_sample_prev - 2.0f * waveform_sample + waveform_sample_next);
        float curvature_mask_value = CLAMP((local_curvature - curvature_min) / (curvature_max - curvature_min), 0.0f, 1.0f);
        curvature_mask_value = curvature_mask_value * curvature_mask_value * (3.0f - 2.0f * curvature_mask_value);

        float waveform_magnitude = FABSF(waveform_sample - analysis_mean);
        float amplitude_mask_value = CLAMP((waveform_magnitude - amplitude_min) / (amplitude_max - amplitude_min), 0.0f, 1.0f);
        amplitude_mask_value = amplitude_mask_value * amplitude_mask_value * (3.0f - 2.0f * amplitude_mask_value);

        float extrema_mask_value = 0.0f;
        if (i > 0 && i < LANE_POINT_COUNT - 1) {
            if ((waveform_sample > waveform_sample_prev && waveform_sample >= waveform_sample_next) ||
                (waveform_sample < waveform_sample_prev && waveform_sample <= waveform_sample_next)) {
                extrema_mask_value = 1.0f;
            }
        }

        float contact_mask_value = POWF(contact_ratio, 1.75f) * curvature_mask_value * amplitude_mask_value * extrema_mask_value;
        contact_mask_values[i] = CLAMP(contact_mask_value, 0.0f, 1.0f);
    }

    for (int i = 0; i < LANE_POINT_COUNT; i++) {
        float contact_mask_value = contact_mask_values[i];

        if (i > 0) {
            contact_mask_value = FMAXF(contact_mask_value, contact_mask_values[i - 1] * neighbor_contact_scale);
        }
        if (i < LANE_POINT_COUNT - 1) {
            contact_mask_value = FMAXF(contact_mask_value, contact_mask_values[i + 1] * neighbor_contact_scale);
        }

        contact_mask_value = CLAMP((contact_mask_value - 0.08f) / (0.82f - 0.08f), 0.0f, 1.0f);
        contact_mask_value = contact_mask_value * contact_mask_value * (3.0f - 2.0f * contact_mask_value);
        hilbert_normal_field[0][i] = POWF(contact_mask_value, 0.65f);
    }
}

static void update_mesh_normals_hilbert(float* normals, const float* src_normals, const float* normal_field, const float* vertices) {
    for (int i = 0; i < LANE_COUNT; i++) {
        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            int k = (i * LANE_POINT_COUNT + j) * 3;
            float contact_mask_value = CLAMP(normal_field[i * LANE_POINT_COUNT + j], 0.0f, 1.0f);

            Vector3 waveform_normal = {
                src_normals[k + 0],
                src_normals[k + 1],
                src_normals[k + 2],
            };
            waveform_normal = Vector3Normalize(waveform_normal);

            Vector3 vertex_position = {
                vertices[k + 0],
                vertices[k + 1],
                vertices[k + 2],
            };
            Vector3 light_direction = Vector3Normalize(Vector3Subtract(light0_position, vertex_position));

            if (Vector3DotProduct(waveform_normal, light_direction) < 0.0f) {
                waveform_normal = Vector3Negate(waveform_normal);
            }

            float light_component = Vector3DotProduct(waveform_normal, light_direction);
            Vector3 cancel_normal = Vector3Subtract(waveform_normal, Vector3Scale(light_direction, light_component));
            cancel_normal = Vector3Normalize(cancel_normal);

            if (contact_mask_value <= 0.0f) {
                normals[k + 0] = cancel_normal.x;
                normals[k + 1] = cancel_normal.y;
                normals[k + 2] = cancel_normal.z;
                continue;
            }

            if (contact_mask_value >= 1.0f) {
                normals[k + 0] = waveform_normal.x;
                normals[k + 1] = waveform_normal.y;
                normals[k + 2] = waveform_normal.z;
                continue;
            }

            contact_mask_value = POWF(contact_mask_value, 0.35f);

            Vector3 final_normal = Vector3Normalize(Vector3Lerp(cancel_normal, waveform_normal, contact_mask_value));
            normals[k + 0] = final_normal.x;
            normals[k + 1] = final_normal.y;
            normals[k + 2] = final_normal.z;
        }
    }
}

static void build_rms_color_field(void) {
    for (int i = 0; i < LANE_POINT_COUNT; i++) {
        float squared_sample_sum = 0.0f;
        int starting_sample_index = i * WAVEFORM_SAMPLES_PER_LANE_POINT;
        for (int j = 0; j < WAVEFORM_SAMPLES_PER_LANE_POINT; j++) {
            int k = starting_sample_index + j;
            float sample_value = analysis_window_samples[k];
            squared_sample_sum += sample_value * sample_value;
        }
        // RMS envelope convention references: https://www.mathworks.com/help/signal/ref/envelope.html and https://en.wikipedia.org/wiki/Root_mean_square
        rms_color_field[0][i] = SQRTF(squared_sample_sum / (float)WAVEFORM_SAMPLES_PER_LANE_POINT);
    }
}

static void update_mesh_colors_rms(Color* colors) {
    for (int i = 0; i < LANE_COUNT; i++) {
        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            int k = i * LANE_POINT_COUNT + j;
            colors[k] = sample_rms_pallete(rms_color_field[i][j]);
        }
    }
}

static Color sample_rms_pallete(float rms) {
    const float visibility_threshold = 0.2f;
    float t = (rms - visibility_threshold) / (1.0f - visibility_threshold);
    if (t <= 0.0f) {
        return (Color){0, 0, 0, 0}; //TODO: figure out a way to group colors, wavefomr is too sparse, fft will work better of course...
    }

    t = CLAMP(t, 0.0f, 1.0f);
    Color color_a = MAGENTA;
    Color color_b = BLUE;
    float blend = t * 3.0f;
    if (t >= (2.0f / 3.0f)) {
        color_a = RED;
        color_b = YELLOW;
        blend = (t - (2.0f / 3.0f)) * 3.0f;
    } else if (t >= (1.0f / 3.0f)) {
        color_a = BLUE;
        color_b = RED;
        blend = (t - (1.0f / 3.0f)) * 3.0f;
    }

    return (Color){
        (unsigned char)LERP((float)color_a.r, (float)color_b.r, blend),
        (unsigned char)LERP((float)color_a.g, (float)color_b.g, blend),
        (unsigned char)LERP((float)color_a.b, (float)color_b.b, blend),
        DRAW_COLOR_CHANNEL_MAX,
    };
}

static void update_onset_interpolation_factor_waveform(void) {
    // raw waveform signal derived onset  LIMITATIONS!!!:
    // https://www.iro.umontreal.ca/~pift6080/H09/documents/papers/bello_onset_tutorial.pdf
    // https://mural.maynoothuniversity.ie/id/eprint/4204/1/JT_Real-Time_Detection.pdf
    float onset_strength = 0.0f;
    for (int i = 0; i < LANE_POINT_COUNT; i++) {
        float row_delta = lane_point_values[0][i] - lane_point_values[1][i];
        onset_strength += row_delta * row_delta;
    }
    onset_strength /= (float)LANE_POINT_COUNT;

    float onset_strength_normalized = CLAMP((onset_strength - ONSET_STRENGTH_MIN) / (ONSET_STRENGTH_MAX - ONSET_STRENGTH_MIN), 0.0f, 1.0f);
    float onset_rate = ONSET_DECAY_RATE;
    if (onset_strength_normalized > onset_interpolation_factor) {
        onset_rate = ONSET_ATTACK_RATE;
    }

    onset_interpolation_factor = LERP(onset_interpolation_factor, onset_strength_normalized, onset_rate);
    onset_interpolation_factor = CLAMP(onset_interpolation_factor, 0.0f, 1.0f);
}

static void update_playback_controls_waveform(void) {
    int analysis_dirty = 0;
    int rebuild_from_cursor = 0;

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
            rebuild_from_cursor = 1;
        } else {
            is_paused = false;
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

                advance_lane_history(&lane_point_values[0][0], LANE_POINT_COUNT);
                advance_lane_history(&hilbert_normal_field[0][0], LANE_POINT_COUNT);
                advance_lane_history(&rms_color_field[0][0], LANE_POINT_COUNT);
                for (int i = 0; i < LANE_POINT_COUNT; i++) {
                    lane_point_values[0][i] = analysis_window_samples[(i * (ANALYSIS_WINDOW_SIZE_IN_FRAMES - 1)) / (LANE_POINT_COUNT - 1)];
                }
                update_onset_interpolation_factor_waveform();
                build_hilbert_normal_field();
                build_rms_color_field();
                analysis_dirty = 1;
            }
        }
    }

    if (is_paused && sticky_nav(GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
        wave_cursor = (wave_cursor + wave.frameCount - AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES) % wave.frameCount;
        seek_delta_chunks--;
        rebuild_from_cursor = 1;
    } else if (is_paused && sticky_nav(GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
        wave_cursor = (wave_cursor + AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES) % wave.frameCount;
        seek_delta_chunks++;
        rebuild_from_cursor = 1;
    }

    if (rebuild_from_cursor) {
        MEMSET(lane_point_values, 0, sizeof(lane_point_values));
        MEMSET(hilbert_normal_field, 0, sizeof(hilbert_normal_field));
        MEMSET(rms_color_field, 0, sizeof(rms_color_field));
        onset_interpolation_factor = 0.0f;

        for (int history_index = LANE_COUNT - 1; history_index >= 0; history_index--) {
            int start_frame = (wave_cursor + wave.frameCount - ((history_index * AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES) % wave.frameCount)) % wave.frameCount;

            for (int sample_index = 0; sample_index < ANALYSIS_WINDOW_SIZE_IN_FRAMES; sample_index++) {
                int src = (start_frame + sample_index) % wave.frameCount;
                analysis_window_samples[sample_index] = (float)wave_pcm16[src] / ANALYSIS_PCM16_UPPER_BOUND;
            }

            advance_lane_history(&lane_point_values[0][0], LANE_POINT_COUNT);
            advance_lane_history(&hilbert_normal_field[0][0], LANE_POINT_COUNT);
            advance_lane_history(&rms_color_field[0][0], LANE_POINT_COUNT);
            for (int i = 0; i < LANE_POINT_COUNT; i++) {
                lane_point_values[0][i] = analysis_window_samples[(i * (ANALYSIS_WINDOW_SIZE_IN_FRAMES - 1)) / (LANE_POINT_COUNT - 1)];
            }
            update_onset_interpolation_factor_waveform();
            build_hilbert_normal_field();
            build_rms_color_field();
        }

        analysis_dirty = 1;
    }

    if (analysis_dirty) {
        update_mesh_vertices(vertices, &lane_point_values[0][0], LANE_POINT_COUNT);
        update_mesh_normals_smooth(normals, vertices, LANE_POINT_COUNT);
        update_mesh_normals_hilbert(hilbert_normals, normals, &hilbert_normal_field[0][0], vertices);
        update_mesh_colors_rms(rms_colors);
        update_mesh_vertices_flat(flat_vertices, vertices, mesh_a.indices);
        update_mesh_normals_flat(flat_normals, flat_vertices);
        expand_mesh_normals_flat(flat_hilbert_normals, hilbert_normals, mesh_a.indices);
        build_mesh_smooth(&mesh_a, vertices, normals, colors, MESH_VERTEX_COUNT);
        build_mesh_smooth(&mesh_b, vertices, normals, rms_colors, MESH_VERTEX_COUNT);
        build_mesh_flat(&flat_mesh, flat_vertices, flat_hilbert_normals, flat_colors);
    }
}
