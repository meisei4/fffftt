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
static float texcoords[MESH_VERTEX_COUNT * 2] = {0};

static float flat_vertices[FLAT_VERTEX_COUNT * 3] = {0};
static float flat_normals[FLAT_VERTEX_COUNT * 3] = {0};
static float flat_hilbert_normals[FLAT_VERTEX_COUNT * 3] = {0};
static Color flat_colors[FLAT_VERTEX_COUNT] = {0};

static void init_hilbert_filter(void);
static void build_hilbert_normal_field(int lane);
static void build_waveform_terrain_lane_from_cursor(int lane, int cursor);
static void update_mesh_normals_hilbert(float* normals, const float* src_normals, const float* normal_field, const float* vertices);
static void build_rms_color_field(int lane);
static void update_mesh_colors_rms(Color* colors);
static Color sample_rms_palette(float rms);

static int inspection_ready = 0;
static void update_playback_controls_waveform(void);
static void recompute_waveform_onset(void);
static void inspection_step(int dir);
static void rebase_waveform_history(void);
static void update_waveform_terrain_meshes(void);
static float adaptive_waveform_onset_power_mean = 0.0f; //TODO: just be clear this is waveform naive onset
static float adaptive_waveform_onset_power_deviation = 0.0f;
static int adaptive_waveform_onset_power_ready = 0;

int main(void) {
    // SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    font = LoadFont(RD_FONT);

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
    set_audio_track(SHADERTOY_EXPERIMENT);
    audio_stream = LoadAudioStream(SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    PlayAudioStream(audio_stream);

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
    Texture2D lane_mask_texture = build_lane_mask(texcoords, LANE_POINT_COUNT);
    wave_cursor_texture = build_lane_mask_glow(texcoords, LANE_POINT_COUNT);

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
    expand_mesh_colors_flat(flat_colors, colors, mesh_a.indices, FLAT_VERTEX_COUNT);
    init_hilbert_filter();
    rebase_waveform_history();
    update_waveform_terrain_meshes();

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        update_audio_track_cycle();
        update_playback_controls_waveform();

        int audio_dirty = 0;
        while (fffftt_audio_process(chunk_samples)) {
            advance_lane_history(&lane_point_values[0][0], LANE_POINT_COUNT);
            advance_lane_history(&hilbert_normal_field[0][0], LANE_POINT_COUNT);
            advance_lane_history(&rms_color_field[0][0], LANE_POINT_COUNT);
            for (int i = 0; i < LANE_POINT_COUNT; i++) {
                lane_point_values[0][i] = analysis_window_samples[(i * (ANALYSIS_WINDOW_SIZE_IN_FRAMES - 1)) / (LANE_POINT_COUNT - 1)];
            }
            build_hilbert_normal_field(0);
            build_rms_color_field(0);
            recompute_waveform_onset();
            audio_dirty = 1;
        }

        if (audio_dirty) {
            update_waveform_terrain_meshes();
        }
        update_camera_orbit(&camera, GetFrameTime());
        update_light_camera_strafe(&camera,
                                   compute_buoy_rest_pos(vertices, LANE_POINT_COUNT, MIDDLE),
                                   (BoundingBox){
                                       .min = {-LIGHT_SCENE_X_HALF, -AMPLITUDE_Y_SCALE - 0.25f, -LIGHT_SCENE_Z_HALF},
                                       .max = {+LIGHT_SCENE_X_HALF, +AMPLITUDE_Y_SCALE + 3.5f, +LIGHT_SCENE_Z_HALF},
                                   });
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
        glLightfv(GL_LIGHT0, GL_POSITION, (const GLfloat[]){light0_pos.x, light0_pos.y, light0_pos.z, 1.0f});
        DrawModelEx(model_a, MIDDLE, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE); //TODO: too powerful...
        glDisable(GL_LIGHTING);
        draw_lantern(light0_pos);
        draw_lantern_glow(light0_pos);

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
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, (const GLfloat[]){0.0f, 0.0f, 0.0f, 0.0f});
        DrawModelEx(flat_model, TOP, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE); //TODO: too powerful...
        glShadeModel(GL_SMOOTH);
        glDisable(GL_LIGHTING);

        rlEnablePointMode();
        rlDisableDepthTest(); // TODO: this happened in v6.0?
        DrawModelEx(model_a, TOP, Y_AXIS, 0.0f, DEFAULT_SCALE, BLUE);
        rlEnableDepthTest();
        rlDisablePointMode();
        DrawModelWiresEx(model_a, BOTTOM, Y_AXIS, 0.0f, DEFAULT_SCALE, MAGENTA);
        if (is_paused) {
            draw_paused_wave_cursor_lane_marker(LANE_POINT_COUNT);
        }

        model_a.meshes[0].colors = saved_colors;

        EndMode3D();
        draw_playback_inspection_hud();
        EndDrawing();
    }

    UnloadTexture(lane_mask_texture);
    UnloadTexture(wave_cursor_texture);
    UnloadModel(model_a);
    UnloadModel(model_b);
    UnloadModel(flat_model);
    UnloadAudioStream(audio_stream);
    unload_audio_track();
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
        kaiser_term *= (kaiser_beta * kaiser_beta) / (4.0f * (float)(i * i));
        kaiser_beta_i0 += kaiser_term;
    }

    for (int j = 1 - hilbert_filter_radius; j <= hilbert_filter_radius - 1; j += 2) {
        float normalized_filter_pos = (float)j / (float)hilbert_filter_radius;
        float kaiser_argument = kaiser_beta * SQRTF(FMAXF(0.0f, 1.0f - normalized_filter_pos * normalized_filter_pos));
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

static void build_hilbert_normal_field(int lane) {
    //Analytic-envelope normal-field mask references:
    //https://www.mathworks.com/help/signal/ref/envelope.html
    // (see all the envelope(,,'analytic') envelopes, the idea is to get the intersection of the envelope and the wavform for ridges...)
    //https://www.mathworks.com/help/signal/ref/hilbert.html
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

        int j_min = MAXI(-(hilbert_filter_radius - 1), center_sample_index - (ANALYSIS_WINDOW_SIZE_IN_FRAMES - 1));
        int j_max = MINI(hilbert_filter_radius - 1, center_sample_index);
        if ((j_min & 1) == 0) {
            j_min++;
        }
        if ((j_max & 1) == 0) {
            j_max--;
        }
        for (int j = j_min; j <= j_max; j += 2) {
            int k = center_sample_index - j;
            float ideal_hilbert = hilbert_filter_ideal[j + hilbert_filter_radius];
            float kaiser_window = hilbert_filter_window[j + hilbert_filter_radius];
            float centered_sample = analysis_window_samples[k] - analysis_mean;
            hilbert_sample += centered_sample * ideal_hilbert * kaiser_window;
        }

        float waveform_sample = lane_point_values[lane][i];
        float real_sample = analysis_window_samples[center_sample_index] - analysis_mean;
        float local_analytic_envelope_value = SQRTF(real_sample * real_sample + hilbert_sample * hilbert_sample);
        float local_upper_envelope_value = analysis_mean + local_analytic_envelope_value;
        float local_lower_envelope_value = analysis_mean - local_analytic_envelope_value;
        float local_envelope_reference_value = (waveform_sample >= analysis_mean) ? local_upper_envelope_value : local_lower_envelope_value;

        float contact_ratio = 1.0f - (FABSF(waveform_sample - local_envelope_reference_value) / local_analytic_envelope_value);
        contact_ratio = CLAMP((contact_ratio - contact_ratio_min) / (1.0f - contact_ratio_min), 0.0f, 1.0f);
        contact_ratio = contact_ratio * contact_ratio * (3.0f - 2.0f * contact_ratio);

        int i_prev = (i > 0) ? i - 1 : i;
        int i_next = (i < LANE_POINT_COUNT - 1) ? i + 1 : i;

        float waveform_sample_prev = lane_point_values[lane][i_prev];
        float waveform_sample_next = lane_point_values[lane][i_next];
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
        hilbert_normal_field[lane][i] = POWF(contact_mask_value, 0.65f);
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

            Vector3 vertex_pos = {
                vertices[k + 0],
                vertices[k + 1],
                vertices[k + 2],
            };
            Vector3 light_direction = Vector3Normalize(Vector3Subtract(light0_pos, vertex_pos));

            if (Vector3DotProduct(waveform_normal, light_direction) < 0.0f) {
                waveform_normal = Vector3Negate(waveform_normal);
            }

            float light_component = Vector3DotProduct(waveform_normal, light_direction);
            Vector3 cancel_normal = Vector3Subtract(waveform_normal, Vector3Scale(light_direction, light_component));
            cancel_normal = Vector3Normalize(cancel_normal);
            contact_mask_value = POWF(contact_mask_value, 0.35f);

            Vector3 final_normal = Vector3Normalize(Vector3Lerp(cancel_normal, waveform_normal, contact_mask_value));
            normals[k + 0] = final_normal.x;
            normals[k + 1] = final_normal.y;
            normals[k + 2] = final_normal.z;
        }
    }
}

static void build_rms_color_field(int lane) {
    for (int i = 0; i < LANE_POINT_COUNT; i++) {
        float squared_sample_sum = 0.0f;
        int starting_sample_index = i * WAVEFORM_SAMPLES_PER_LANE_POINT;
        for (int j = 0; j < WAVEFORM_SAMPLES_PER_LANE_POINT; j++) {
            int k = starting_sample_index + j;
            float sample_value = analysis_window_samples[k];
            squared_sample_sum += sample_value * sample_value;
        }
        // RMS envelope convention references: https://www.mathworks.com/help/signal/ref/envelope.html and https://en.wikipedia.org/wiki/Root_mean_square
        rms_color_field[lane][i] = SQRTF(squared_sample_sum / (float)WAVEFORM_SAMPLES_PER_LANE_POINT);
    }
}

static void update_mesh_colors_rms(Color* colors) {
    MEMSET(colors, 0, sizeof(Color) * MESH_VERTEX_COUNT);
    for (int i = 0; i < LANE_COUNT; i++) {
        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            float rms = rms_color_field[i][j];
            colors[i * LANE_POINT_COUNT + j] = sample_rms_palette(rms);
        }
    }
}

static Color sample_rms_palette(float rms) {
    float t = (rms - 0.2f) / (1.0f - 0.2f);
    if (t <= 0.0f) {
        return BLANK;
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

static void build_waveform_terrain_lane_from_cursor(int lane, int cursor) {
    fffftt_inspection_fill_analysis_window(cursor);

    for (int i = 0; i < LANE_POINT_COUNT; i++) {
        lane_point_values[lane][i] = analysis_window_samples[(i * (ANALYSIS_WINDOW_SIZE_IN_FRAMES - 1)) / (LANE_POINT_COUNT - 1)];
    }

    build_hilbert_normal_field(lane);
    build_rms_color_field(lane);
}

static void recompute_waveform_onset(void) {
    // raw waveform signal derived onset  LIMITATIONS!!!:
    // https://www.iro.umontreal.ca/~pift6080/H09/documents/papers/bello_onset_tutorial.pdf
    // https://mural.maynoothuniversity.ie/id/eprint/4204/1/JT_Real-Time_Detection.pdf
    float onset_delta_power = 0.0f;

    for (int j = 0; j < LANE_POINT_COUNT; j++) {
        float lane_delta = FMAXF(FABSF(lane_point_values[0][j]) - FABSF(lane_point_values[1][j]), 0.0f);
        onset_delta_power += lane_delta * lane_delta;
    }
    onset_delta_power /= (float)LANE_POINT_COUNT;

    if (!adaptive_waveform_onset_power_ready) {
        adaptive_waveform_onset_power_mean = onset_delta_power;
        adaptive_waveform_onset_power_deviation = onset_delta_power;
        adaptive_waveform_onset_power_ready = 1;
    } else {
        float adaptive_waveform_onset_power_delta = FABSF(onset_delta_power - adaptive_waveform_onset_power_mean);
        adaptive_waveform_onset_power_mean = LERP(adaptive_waveform_onset_power_mean, onset_delta_power, GLOBAL_ADAPTIVE_GATE_SMOOTHING_FACTOR);
        adaptive_waveform_onset_power_deviation =
            LERP(adaptive_waveform_onset_power_deviation, adaptive_waveform_onset_power_delta, GLOBAL_ADAPTIVE_GATE_SMOOTHING_FACTOR);
    }

    float adaptive_waveform_onset_power_floor =
        adaptive_waveform_onset_power_mean - adaptive_waveform_onset_power_deviation * GLOBAL_ADAPTIVE_GATE_FLOOR_DEVIATION_SCALE;
    float adaptive_waveform_onset_power_ceiling =
        adaptive_waveform_onset_power_mean + adaptive_waveform_onset_power_deviation * GLOBAL_ADAPTIVE_GATE_CEILING_DEVIATION_SCALE;
    float onset_gate_target = 0.0f;
    if (adaptive_waveform_onset_power_ceiling > adaptive_waveform_onset_power_floor) {
        onset_gate_target =
            CLAMP((onset_delta_power - adaptive_waveform_onset_power_floor) / (adaptive_waveform_onset_power_ceiling - adaptive_waveform_onset_power_floor),
                  0.0f,
                  1.0f);
    }
    if (onset_gate_target > onset_gate) {
        onset_gate = onset_gate_target;
    } else {
        onset_gate = LERP(onset_gate, onset_gate_target, ONSET_DECAY_RATE);
    }
    onset_gate = CLAMP(onset_gate, 0.0f, 1.0f);
}

static void inspection_step(int dir) {
    for (int i = (dir == FORWARD) ? LANE_COUNT - 1 : 0; i != ((dir == FORWARD) ? 0 : LANE_COUNT - 1); i -= dir) {
        MEMCPY(lane_point_values[i], lane_point_values[i - dir], sizeof(lane_point_values[i]));
        MEMCPY(hilbert_normal_field[i], hilbert_normal_field[i - dir], sizeof(hilbert_normal_field[i]));
        MEMCPY(rms_color_field[i], rms_color_field[i - dir], sizeof(rms_color_field[i]));
    }

    const int target_lane = (dir == FORWARD) ? 0 : LANE_COUNT - 1;
    const int back_lane_offset = WRAP((LANE_COUNT - 1) * AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES, wave.frameCount);
    const int target_cursor = (dir == FORWARD) ? wave_cursor : WRAP_MINUS(wave_cursor, back_lane_offset, wave.frameCount);
    build_waveform_terrain_lane_from_cursor(target_lane, target_cursor);
    recompute_waveform_onset();
}

static void rebase_waveform_history(void) {
    inspection_ready = 0;
    MEMSET(lane_point_values, 0, sizeof(lane_point_values));
    MEMSET(hilbert_normal_field, 0, sizeof(hilbert_normal_field));
    MEMSET(rms_color_field, 0, sizeof(rms_color_field));
    onset_gate = 0.0f;
    adaptive_waveform_onset_power_mean = 0.0f;
    adaptive_waveform_onset_power_deviation = 0.0f;
    adaptive_waveform_onset_power_ready = 0;

    for (int i = 0; i < LANE_COUNT; i++) {
        int lane_cursor = WRAP_MINUS(wave_cursor, i * AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES, wave.frameCount);
        build_waveform_terrain_lane_from_cursor(i, lane_cursor);
    }

    recompute_waveform_onset();
    inspection_ready = 1;
}

static void update_waveform_terrain_meshes(void) {
    update_mesh_vertices(vertices, &lane_point_values[0][0], LANE_POINT_COUNT, AMPLITUDE_Y_SCALE);
    update_mesh_normals_smooth(normals, vertices, LANE_POINT_COUNT);
    update_mesh_normals_hilbert(hilbert_normals, normals, &hilbert_normal_field[0][0], vertices);
    update_mesh_colors_rms(rms_colors);

    update_mesh_vertices_flat(flat_vertices, vertices, mesh_a.indices, FLAT_VERTEX_COUNT);
    update_mesh_normals_flat(flat_normals, flat_vertices, TERRAIN_TRIANGLE_COUNT);
    expand_mesh_normals_flat(flat_hilbert_normals, hilbert_normals, mesh_a.indices, FLAT_VERTEX_COUNT);

    // build_mesh_smooth(&mesh_a, vertices, normals, colors);
    build_mesh_smooth(&mesh_a, vertices, normals, colors, texcoords, MESH_VERTEX_COUNT);
    // build_mesh_smooth(&mesh_a, vertices, hilbert_normals, colors);

    // build_mesh_smooth(&mesh_b, vertices, normals, colors);
    build_mesh_smooth(&mesh_b, vertices, normals, rms_colors, mesh_b.texcoords, MESH_VERTEX_COUNT);

    //TODO: ugly hack to get occasional transparency on top layer without inventing a whole new color field...
    for (int i = 0; i < FLAT_VERTEX_COUNT; i++) {
        flat_colors[i] = ((&hilbert_normal_field[0][0])[mesh_a.indices[i]] <= 0.2f) ? BLANK : colors[mesh_a.indices[i]];
    }
    // build_mesh_flat(&flat_mesh, flat_vertices, flat_normals, flat_colors, FLAT_VERTEX_COUNT);
    // build_mesh_flat(&flat_mesh, flat_vertices, flat_hilbert_normals, flat_colors, FLAT_VERTEX_COUNT);
    build_mesh_flat(&flat_mesh, flat_vertices, flat_hilbert_normals, flat_colors, FLAT_VERTEX_COUNT);
}

static void update_playback_controls_waveform(void) {
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
            rebase_waveform_history();
            update_waveform_terrain_meshes();
        } else {
            is_paused = false;
            inspection_ready = 0;
            PlayAudioStream(audio_stream);
            while (fffftt_audio_process(resume_chunk_samples)) {
                advance_lane_history(&lane_point_values[0][0], LANE_POINT_COUNT);
                advance_lane_history(&hilbert_normal_field[0][0], LANE_POINT_COUNT);
                advance_lane_history(&rms_color_field[0][0], LANE_POINT_COUNT);
                for (int i = 0; i < LANE_POINT_COUNT; i++) {
                    lane_point_values[0][i] = analysis_window_samples[(i * (ANALYSIS_WINDOW_SIZE_IN_FRAMES - 1)) / (LANE_POINT_COUNT - 1)];
                }
                build_hilbert_normal_field(0);
                build_rms_color_field(0);
                recompute_waveform_onset();
                analysis_dirty = 1;
            }
        }
    }

    if (is_paused && sticky_nav(GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
        wave_cursor = WRAP_MINUS(wave_cursor, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES, wave.frameCount);
        seek_delta_chunks--;
        if (inspection_ready) {
            inspection_step(BACKWARD);
        } else {
            rebase_waveform_history();
        }
        update_waveform_terrain_meshes();
    } else if (is_paused && sticky_nav(GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
        wave_cursor = WRAP_PLUS(wave_cursor, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES, wave.frameCount);
        seek_delta_chunks++;
        if (inspection_ready) {
            inspection_step(FORWARD);
        } else {
            rebase_waveform_history();
        }
        update_waveform_terrain_meshes();
    }

    if (analysis_dirty) {
        update_waveform_terrain_meshes();
    }
}
