#define FFFFTT_PROFILE_WAVEFORM_TERRAIN_3D
#include "fffftt.h"
#include "raylib.h"
#include "rlgl.h"
#include <GL/gl.h>
#include <string.h>

#define TOP (Vector3){0.0f, 2.0f, 0.0f}
#define MIDDLE (Vector3){0.0f}
#define BOTTOM (Vector3){0.0f, -1.0f, 0.0f}

#define LINE_WIDTH_RASTER_PIXELS 1.0f
#define POINT_SIZE_RASTER_PIXELS 3.0f
#define ONSET_LIGHT_GAIN 28.0f
#define ONSET_LIGHT_RELEASE 0.05f
#define TERRAIN_TRIANGLE_COUNT (((LANE_COUNT - 1) * (LANE_POINT_COUNT - 1)) * 2)
#define FLAT_VERTEX_COUNT (TERRAIN_TRIANGLE_COUNT * 3)

static const char* domain = "WAVEFORM-TERRAIN-3D-DC";

static Mesh mesh_a = {0};
static Mesh mesh_b = {0};
static Mesh flat_mesh = {0};

static Model model_a = {0};
static Model model_b = {0};
static Model flat_model = {0};

static void expand_mesh_normals_flat(float* dst_normals, const float* src_normals, const unsigned short* indices);
static void expand_mesh_colors_flat(Color* dst_colors, const Color* src_colors, const unsigned short* indices);

static void update_mesh_vertices_flat(float* dst_vertices, const float* src_vertices, const unsigned short* indices);
static void update_mesh_normals_flat(float* normals, const float* vertices);
static void update_mesh_normals_smooth(float* normals, const float* vertices);
static void update_mesh_normals_hilbert(float* normals, const float* src_normals, const float* normal_field, const float* vertices);
static void update_mesh_colors_rms(Color* colors);

static void init_hilbert_filter(void);
static void build_hilbert_normal_field(void);
static void build_rms_color_field(void);
static Color sample_rms_pallete(float rms);
static void update_light_constants(void);

static void build_mesh_smooth(Mesh* dst_mesh, const float* src_normals, const Color* src_colors);
static void build_mesh_flat(Mesh* dst_mesh, const float* src_normals, const Color* src_colors);

static float lane_point_values[LANE_COUNT][LANE_POINT_COUNT] = {0};
static float analysis_window_samples[ANALYSIS_WINDOW_SIZE_IN_FRAMES] = {0};
static float hilbert_normal_field[LANE_COUNT][LANE_POINT_COUNT] = {0};
static float rms_color_field[LANE_COUNT][LANE_POINT_COUNT] = {0};
static int hilbert_center_sample_indices[LANE_POINT_COUNT] = {0};
static float hilbert_filter_ideal[65] = {0};
static float hilbert_filter_window[65] = {0};
static float onset_attack_pulse = 0.0f;
static float onset_attack_light_modulator = 0.0f;
static GLfloat light0_ambient[4] = {0.01f, 0.01f, 0.01f, 1.0f};
static GLfloat light0_diffuse[4] = {0.16f, 0.16f, 0.16f, 1.0f};

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
    int16_t chunk_samples[AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES] = {0};

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
    // Wave wave = LoadWave(RD_DDS_FFM_22K_WAV);
    Wave wave = LoadWave(RD_SHADERTOY_EXPERIMENT_22K_WAV);
    WaveFormat(&wave, SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    AudioStream audio_stream = LoadAudioStream(SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    PlayAudioStream(audio_stream);

    int wave_cursor = 0;
    int16_t* wave_pcm16 = (int16_t*)wave.data;

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
    Texture2D lane_mask_texture = build_lane_mask(mesh_a.texcoords);

    model_a = LoadModelFromMesh(mesh_a);
    model_a.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = lane_mask_texture;
    int saved_texture_id = model_a.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id;
    model_a.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = 0;

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

    fill_mesh_colors(colors);
    expand_mesh_colors_flat(flat_colors, colors, mesh_a.indices);
    init_hilbert_filter();

    update_terrain_mesh_vertices(vertices, &lane_point_values[0][0]);
    update_mesh_normals_smooth(normals, vertices);

    build_hilbert_normal_field();
    update_mesh_normals_hilbert(hilbert_normals, normals, &hilbert_normal_field[0][0], vertices);
    update_mesh_colors_rms(rms_colors);

    update_mesh_vertices_flat(flat_vertices, vertices, mesh_a.indices);
    update_mesh_normals_flat(flat_normals, flat_vertices);
    expand_mesh_normals_flat(flat_hilbert_normals, hilbert_normals, mesh_a.indices);

    // build_mesh_smooth(&mesh_a, normals, colors);
    build_mesh_smooth(&mesh_a, normals, colors);
    // build_mesh_smooth(&mesh_a, hilbert_normals, colors);

    // build_mesh_smooth(&mesh_b, normals, colors);
    build_mesh_smooth(&mesh_b, normals, rms_colors);

    // build_mesh_flat(&flat_mesh, flat_normals, flat_colors);
    // build_mesh_flat(&flat_mesh, flat_hilbert_normals, flat_colors);
    build_mesh_flat(&flat_mesh, flat_hilbert_normals, flat_colors);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        int audio_dirty = 0;
        while (IsAudioStreamProcessed(audio_stream)) {
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

            advance_lane_history(&lane_point_values[0][0]);
            advance_lane_history(&hilbert_normal_field[0][0]);
            advance_lane_history(&rms_color_field[0][0]);
            for (int i = 0; i < LANE_POINT_COUNT; i++) {
                lane_point_values[0][i] = analysis_window_samples[(i * (ANALYSIS_WINDOW_SIZE_IN_FRAMES - 1)) / (LANE_POINT_COUNT - 1)];
            }
            float onset_row_delta = 0.0f;
            for (int i = 0; i < LANE_POINT_COUNT; i++) {
                float row_delta = lane_point_values[0][i] - lane_point_values[1][i];
                onset_row_delta += row_delta * row_delta;
            }
            onset_row_delta /= (float)LANE_POINT_COUNT;
            float target_attack_pulse = CLAMP(onset_row_delta * ONSET_LIGHT_GAIN, 0.0f, 1.0f);
            if (target_attack_pulse > onset_attack_pulse) {
                onset_attack_pulse = target_attack_pulse;
            }
            build_hilbert_normal_field();
            build_rms_color_field();
            audio_dirty = 1;
        }

        if (audio_dirty) {
            update_terrain_mesh_vertices(vertices, &lane_point_values[0][0]);
            update_mesh_normals_smooth(normals, vertices);
            update_mesh_normals_hilbert(hilbert_normals, normals, &hilbert_normal_field[0][0], vertices);
            update_mesh_colors_rms(rms_colors);
            update_mesh_vertices_flat(flat_vertices, vertices, mesh_a.indices);
            expand_mesh_normals_flat(flat_hilbert_normals, hilbert_normals, mesh_a.indices);

            build_mesh_smooth(&mesh_a, normals, colors);
            // build_mesh_smooth(&mesh_a, hilbert_normals, colors);

            // build_mesh_smooth(&mesh_b, normals, colors);
            build_mesh_smooth(&mesh_b, normals, rms_colors);

            // build_mesh_flat(&flat_mesh, flat_normals, flat_colors);
            build_mesh_flat(&flat_mesh, flat_hilbert_normals, flat_colors);
        }

        update_camera_orbit(&camera, GetFrameTime());
        update_light_constants();

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);
        rlSetLineWidth(LINE_WIDTH_RASTER_PIXELS);
        rlSetPointSize(POINT_SIZE_RASTER_PIXELS);

        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (const GLfloat[]){0.32f, 0.32f, 0.32f, 1.0f});
        //base:
        // glLightfv(GL_LIGHT0, GL_AMBIENT, (const GLfloat[]){0.08f, 0.08f, 0.08f, 1.0f});
        // glLightfv(GL_LIGHT0, GL_DIFFUSE, (const GLfloat[]){0.60f, 0.60f, 0.60f, 1.0f});
        //onset modulation:
        glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
        glLightfv(GL_LIGHT0, GL_POSITION, (const GLfloat[]){1.330f, 1.345f, -1.418f, 1.0f}); // TODO: same issue with the camera position manual derivation
        DrawModelEx(model_a, MIDDLE, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);

        glShadeModel(GL_FLAT);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (const GLfloat[]){0.0f, 0.0f, 0.0f, 1.0f});
        glLightfv(GL_LIGHT0, GL_AMBIENT, (const GLfloat[]){0.0f, 0.0f, 0.0f, 1.0f});
        glLightfv(GL_LIGHT0, GL_DIFFUSE, (const GLfloat[]){1.0f, 1.0f, 1.0f, 1.0f});
        DrawModelEx(flat_model, TOP, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);
        glShadeModel(GL_SMOOTH);
        glDisable(GL_LIGHTING);
        // glDisable(GL_LIGHT0);
        // glDisable(GL_COLOR_MATERIAL);

        DrawModelEx(model_b, BOTTOM, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);
        unsigned char* saved_colors = model_a.meshes[0].colors;

        model_a.meshes[0].colors = NULL;
        // DrawModelWiresEx(model, MIDDLE, Y_AXIS, 0.0f, DEFAULT_SCALE, BLUE);
        // DrawModelPointsEx(model, Y_AXIS, Y_AXIS, 0.0f, DEFAULT_SCALE, MAGENTA);
        DrawModelPointsEx(model_a, TOP, Y_AXIS, 0.0f, DEFAULT_SCALE, BLUE);
        DrawModelWiresEx(model_a, BOTTOM, Y_AXIS, 0.0f, DEFAULT_SCALE, MAGENTA);

        model_a.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = saved_texture_id;
        rlDisableDepthMask();
        rlDisableBackfaceCulling();
        DrawModelEx(model_a, MIDDLE, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);
        rlEnableBackfaceCulling();
        rlEnableDepthMask();

        model_a.meshes[0].colors = saved_colors;
        model_a.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = 0;

        EndMode3D();
        // DrawFPS(550, 440);
        EndDrawing();
    }

    UnloadTexture(lane_mask_texture);
    UnloadModel(model_a);
    UnloadModel(model_b);
    UnloadModel(flat_model);
    UnloadAudioStream(audio_stream);
    UnloadWave(wave);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

static void update_mesh_vertices_flat(float* dst_vertices, const float* src_vertices, const unsigned short* indices) {
    for (int i = 0; i < FLAT_VERTEX_COUNT; i++) {
        int src_index = indices[i];
        int src_vertex = src_index * 3;
        int dst_vertex = i * 3;
        dst_vertices[dst_vertex + 0] = src_vertices[src_vertex + 0];
        dst_vertices[dst_vertex + 1] = src_vertices[src_vertex + 1];
        dst_vertices[dst_vertex + 2] = src_vertices[src_vertex + 2];
    }
}

static void expand_mesh_colors_flat(Color* dst_colors, const Color* src_colors, const unsigned short* indices) {
    for (int i = 0; i < FLAT_VERTEX_COUNT; i++) {
        int src_index = indices[i];
        dst_colors[i] = src_colors[src_index];
    }
}

static void expand_mesh_normals_flat(float* dst_normals, const float* src_normals, const unsigned short* indices) {
    for (int i = 0; i < FLAT_VERTEX_COUNT; i++) {
        int src_index = indices[i];
        int src_normal = src_index * 3;
        int dst_normal = i * 3;
        dst_normals[dst_normal + 0] = src_normals[src_normal + 0];
        dst_normals[dst_normal + 1] = src_normals[src_normal + 1];
        dst_normals[dst_normal + 2] = src_normals[src_normal + 2];
    }
}

static void update_mesh_normals_flat(float* normals, const float* vertices) {
    for (int i = 0; i < TERRAIN_TRIANGLE_COUNT; i++) {
        int tri_vertex = i * 9;
        Vector3 p0 = {vertices[tri_vertex + 0], vertices[tri_vertex + 1], vertices[tri_vertex + 2]};
        Vector3 p1 = {vertices[tri_vertex + 3], vertices[tri_vertex + 4], vertices[tri_vertex + 5]};
        Vector3 p2 = {vertices[tri_vertex + 6], vertices[tri_vertex + 7], vertices[tri_vertex + 8]};
        Vector3 e1 = Vector3Subtract(p1, p0);
        Vector3 e2 = Vector3Subtract(p2, p0);
        Vector3 normal = Vector3Normalize(Vector3CrossProduct(e1, e2));

        for (int j = 0; j < 3; j++) {
            int normal_vertex = tri_vertex + j * 3;
            normals[normal_vertex + 0] = normal.x;
            normals[normal_vertex + 1] = normal.y;
            normals[normal_vertex + 2] = normal.z;
        }
    }
}

static void update_mesh_normals_smooth(float* normals, const float* vertices) {
    for (int i = 0; i < LANE_COUNT; i++) {
        int i_prev = (i > 0) ? i - 1 : i;
        int i_next = (i < LANE_COUNT - 1) ? i + 1 : i;

        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            int j_prev = (j > 0) ? j - 1 : j;
            int j_next = (j < LANE_POINT_COUNT - 1) ? j + 1 : j;

            int k_left = (i * LANE_POINT_COUNT + j_prev) * 3;
            int k_right = (i * LANE_POINT_COUNT + j_next) * 3;
            int k_back = (i_prev * LANE_POINT_COUNT + j) * 3;
            int k_front = (i_next * LANE_POINT_COUNT + j) * 3;
            int k_out = (i * LANE_POINT_COUNT + j) * 3;

            //TODO: is there a more standarized way to do this? like can we anticipate tangents in dynamic vertex attributes or something???
            float tangent_x_x = vertices[k_right + 0] - vertices[k_left + 0];
            float tangent_x_y = vertices[k_right + 1] - vertices[k_left + 1];
            float tangent_z_y = vertices[k_front + 1] - vertices[k_back + 1];
            float tangent_z_z = vertices[k_front + 2] - vertices[k_back + 2];
            Vector3 n = Vector3Normalize((Vector3){-tangent_z_z * tangent_x_y, tangent_z_z * tangent_x_x, -tangent_z_y * tangent_x_x});

            normals[k_out + 0] = n.x;
            normals[k_out + 1] = n.y;
            normals[k_out + 2] = n.z;
        }
    }
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
    const float denominator_epsilon = 1e-12f;
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

        float contact_ratio = 1.0f - (FABSF(waveform_sample - local_envelope_reference_value) / FMAXF(local_analytic_envelope_value, denominator_epsilon));
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
    const Vector3 light_position = {1.330f, 1.345f, -1.418f};

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
            Vector3 light_direction = Vector3Normalize(Vector3Subtract(light_position, vertex_position));

            if (Vector3DotProduct(waveform_normal, light_direction) < 0.0f) {
                waveform_normal = Vector3Negate(waveform_normal);
            }

            float light_component = Vector3DotProduct(waveform_normal, light_direction);
            Vector3 cancel_normal = Vector3Subtract(waveform_normal, Vector3Scale(light_direction, light_component));
            float cancel_normal_length = Vector3Length(cancel_normal);

            if (cancel_normal_length <= 1e-6f) {
                Vector3 fallback_axis = {0.0f, 1.0f, 0.0f};
                if (FABSF(light_direction.y) > 0.98f) {
                    fallback_axis = (Vector3){1.0f, 0.0f, 0.0f};
                }
                cancel_normal = Vector3Normalize(Vector3CrossProduct(light_direction, fallback_axis));
            } else {
                cancel_normal = Vector3Scale(cancel_normal, 1.0f / cancel_normal_length);
            }

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

static void update_light_constants(void) {
    // raw waveform signal derived onset  LIMITATIONS!!!:
    // https://www.iro.umontreal.ca/~pift6080/H09/documents/papers/bello_onset_tutorial.pdf
    // https://mural.maynoothuniversity.ie/id/eprint/4204/1/JT_Real-Time_Detection.pdf
    // TODO: LATER FOR FFT/SPECTRUM ONSET STUFF!!!!!!!!!!!:
    // https://librosa.org/doc/main/generated/librosa.onset.onset_strength.html
    // https://librosa.org/doc/main/generated/librosa.onset.onset_detect.html
    float target_attack_pulse = onset_attack_pulse;
    onset_attack_pulse = 0.0f;
    if (target_attack_pulse > onset_attack_light_modulator) {
        onset_attack_light_modulator = target_attack_pulse;
    } else {
        onset_attack_light_modulator = LERP(onset_attack_light_modulator, 0.0f, ONSET_LIGHT_RELEASE);
    }
    const float base_light0_ambient = 0.01f;
    const float base_light0_diffuse = 0.16f;
    const float ambient_pulse_range = 0.01f;
    const float diffuse_pulse_range = 0.82f;
    float ambient_strength = base_light0_ambient + onset_attack_light_modulator * ambient_pulse_range;
    float diffuse_strength = base_light0_diffuse + onset_attack_light_modulator * diffuse_pulse_range;
    light0_ambient[0] = ambient_strength;
    light0_ambient[1] = ambient_strength;
    light0_ambient[2] = ambient_strength;
    light0_ambient[3] = 1.0f;
    light0_diffuse[0] = diffuse_strength;
    light0_diffuse[1] = diffuse_strength;
    light0_diffuse[2] = diffuse_strength;
    light0_diffuse[3] = 1.0f;
}

static void build_mesh_smooth(Mesh* dst_mesh, const float* src_normals, const Color* src_colors) {
    MEMCPY4(dst_mesh->vertices, vertices, sizeof(float) * MESH_VERTEX_COUNT * 3);
    MEMCPY4(dst_mesh->normals, src_normals, sizeof(float) * MESH_VERTEX_COUNT * 3);
    MEMCPY(dst_mesh->colors, src_colors, sizeof(Color) * MESH_VERTEX_COUNT);
}

static void build_mesh_flat(Mesh* dst_mesh, const float* src_normals, const Color* src_colors) {
    MEMCPY4(dst_mesh->vertices, flat_vertices, sizeof(float) * FLAT_VERTEX_COUNT * 3);
    MEMCPY4(dst_mesh->normals, src_normals, sizeof(float) * FLAT_VERTEX_COUNT * 3);
    MEMCPY(dst_mesh->colors, src_colors, sizeof(Color) * FLAT_VERTEX_COUNT);
}
