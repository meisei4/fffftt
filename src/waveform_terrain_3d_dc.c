#define FFFFTT_PROFILE_WAVEFORM_TERRAIN_3D
#include "fffftt.h"
#include "raylib.h"
#include "rlgl.h"
#include <GL/gl.h>

#define LINE_WIDTH_RASTER_PIXELS 1.0f
#define POINT_SIZE_RASTER_PIXELS 3.0f

static const char* domain = "WAVEFORM-TERRAIN-3D-DC";

static Mesh mesh = {0};
static Mesh flat_mesh = {0};
static void update_flat_mesh_vertices(Mesh* dst_mesh, const Mesh* src_mesh);
static void fill_flat_mesh_colors(Mesh* dst_mesh, const Mesh* src_mesh);
static void update_flat_mesh_normals(Mesh* mesh);
static void update_smooth_mesh_normals(Mesh* mesh);
static float lane_point_values[LANE_COUNT][LANE_POINT_COUNT] = {0};
static float analysis_window_samples[ANALYSIS_WINDOW_SIZE_IN_FRAMES] = {0};

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

    unsigned int wave_cursor = 0;
    int16_t* wave_pcm16 = (int16_t*)wave.data;

    Camera3D camera = {
        .position = (Vector3){1.456f, 1.345f, -1.366f},
        .target = (Vector3){0.0f, 0.25f, 0.0f},
        .up = (Vector3){0.0f, 1.0f, 0.0f},
        .fovy = 3.111f,
        .projection = CAMERA_ORTHOGRAPHIC,
    };

    mesh = GenMeshPlane(1.0f, 1.0f, LANE_POINT_COUNT - 1, LANE_COUNT - 1);
    mesh.colors = RL_CALLOC(mesh.vertexCount, sizeof(Color));

    update_terrain_mesh_vertices(mesh.vertices, &lane_point_values[0][0]);
    update_smooth_mesh_normals(&mesh);
    // fill_mesh_indices_lane_topology(mesh.indices); //TODO: this would be for a direct drawElements call not a mesh draw, custom topology is a bit complicated
    fill_mesh_colors((Color*)mesh.colors);

    Model model = LoadModelFromMesh(mesh);
    Mesh* draw_mesh = &model.meshes[0];

    flat_mesh = (Mesh){
        .vertexCount = mesh.triangleCount * 3,
        .triangleCount = mesh.triangleCount,
        .vertices = RL_CALLOC(mesh.triangleCount * 3 * 3, sizeof(float)),
        .normals = RL_CALLOC(mesh.triangleCount * 3 * 3, sizeof(float)),
        .colors = RL_CALLOC(mesh.triangleCount * 3, sizeof(Color)),
        .indices = NULL, //NOTE: explicit here for didactic
    };
    update_flat_mesh_vertices(&flat_mesh, &mesh);
    update_flat_mesh_normals(&flat_mesh);
    fill_flat_mesh_colors(&flat_mesh, &mesh);
    Model flat_model = LoadModelFromMesh(flat_mesh);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

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
            for (int i = 0; i < LANE_POINT_COUNT; i++) {
                lane_point_values[0][i] = analysis_window_samples[(i * (ANALYSIS_WINDOW_SIZE_IN_FRAMES - 1)) / (LANE_POINT_COUNT - 1)];
            }
        }

        update_terrain_mesh_vertices(mesh.vertices, &lane_point_values[0][0]);
        update_smooth_mesh_normals(&mesh);
        update_flat_mesh_vertices(&flat_mesh, &mesh);
        update_flat_mesh_normals(&flat_mesh);
        update_camera_orbit(&camera, GetFrameTime());

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);
        rlSetLineWidth(LINE_WIDTH_RASTER_PIXELS);
        rlSetPointSize(POINT_SIZE_RASTER_PIXELS);

        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (const GLfloat[]){0.32f, 0.32f, 0.32f, 1.0f});
        glLightfv(GL_LIGHT0, GL_AMBIENT, (const GLfloat[]){0.08f, 0.08f, 0.08f, 1.0f});
        glLightfv(GL_LIGHT0, GL_DIFFUSE, (const GLfloat[]){0.60f, 0.60f, 0.60f, 1.0f});
        glLightfv(GL_LIGHT0, GL_POSITION, (const GLfloat[]){1.330f, 1.345f, -1.418f, 1.0f}); // TODO: same issue with the camera position manual derivation
        DrawModelEx(model, (Vector3){0.0f}, (Vector3){0.0f, 1.0f, 0.0f}, 0.0f, (Vector3){1.0f, 1.0f, 1.0f}, WHITE);

        glShadeModel(GL_FLAT);
        DrawModelEx(flat_model, (Vector3){0.0, -1.0f, 0.0f}, (Vector3){0.0f, 1.0f, 0.0f}, 0.0f, (Vector3){1.0f, 1.0f, 1.0f}, WHITE);
        glShadeModel(GL_SMOOTH);
        glDisable(GL_LIGHTING);

        DrawModelEx(model, (Vector3){0.0f, 1.0f, 0.0f}, (Vector3){0.0f, 1.0f, 0.0f}, 0.0f, (Vector3){1.0f, 1.0f, 1.0f}, WHITE);
        unsigned char* saved_colors = draw_mesh->colors;

        draw_mesh->colors = NULL;
        DrawModelWiresEx(model, (Vector3){0.0f, 0.0f, 0.0f}, (Vector3){0.0f, 1.0f, 0.0f}, 0.0f, (Vector3){1.0f, 1.0f, 1.0f}, BLUE);
        DrawModelPointsEx(model, (Vector3){0.0f, -1.0f, 0.0f}, (Vector3){0.0f, 1.0f, 0.0f}, 0.0f, (Vector3){1.0f, 1.0f, 1.0f}, MAGENTA);
        draw_mesh->colors = saved_colors;

        EndMode3D();
        // DrawFPS(540, 420);
        EndDrawing();
    }

    UnloadModel(model);
    UnloadModel(flat_model);
    UnloadAudioStream(audio_stream);
    UnloadWave(wave);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

static void update_flat_mesh_vertices(Mesh* dst_mesh, const Mesh* src_mesh) {
    const float* src_vertices = src_mesh->vertices;
    float* dst_vertices = dst_mesh->vertices;

    for (int i = 0; i < dst_mesh->vertexCount; i++) {
        int src_index = src_mesh->indices[i];
        int src_vertex = src_index * 3;
        int dst_vertex = i * 3;
        dst_vertices[dst_vertex + 0] = src_vertices[src_vertex + 0];
        dst_vertices[dst_vertex + 1] = src_vertices[src_vertex + 1];
        dst_vertices[dst_vertex + 2] = src_vertices[src_vertex + 2];
    }
}

static void fill_flat_mesh_colors(Mesh* dst_mesh, const Mesh* src_mesh) {
    const Color* src_colors = (const Color*)src_mesh->colors;
    Color* dst_colors = (Color*)dst_mesh->colors;

    for (int i = 0; i < dst_mesh->vertexCount; i++) {
        int src_index = src_mesh->indices[i];
        dst_colors[i] = src_colors[src_index];
    }
}

static void update_flat_mesh_normals(Mesh* mesh) {
    float* vertices = mesh->vertices;
    float* normals = mesh->normals;

    for (int i = 0; i < mesh->triangleCount; i++) {
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

static void update_smooth_mesh_normals(Mesh* mesh) {
    float* vertices = mesh->vertices;
    float* normals = mesh->normals;

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

            Vector3 left = {vertices[k_left + 0], vertices[k_left + 1], vertices[k_left + 2]};
            Vector3 right = {vertices[k_right + 0], vertices[k_right + 1], vertices[k_right + 2]};
            Vector3 back = {vertices[k_back + 0], vertices[k_back + 1], vertices[k_back + 2]};
            Vector3 front = {vertices[k_front + 0], vertices[k_front + 1], vertices[k_front + 2]};

            //TODO: is there a more standarized way to do this? like can we anticipate tangents in dynamic vertex attributes or something???
            Vector3 tangent_x = Vector3Subtract(right, left);
            Vector3 tangent_z = Vector3Subtract(front, back);
            Vector3 n = Vector3Normalize(Vector3CrossProduct(tangent_z, tangent_x));

            normals[k_out + 0] = n.x;
            normals[k_out + 1] = n.y;
            normals[k_out + 2] = n.z;
        }
    }
}
