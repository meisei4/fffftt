#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define GRID_Z 0.0f
#define INDEXED_ROW_Y 0.9f
#define UNINDEXED_ROW_Y -0.9f
#define FILL_COLUMN_X -1.4f
#define WIRE_COLUMN_X 0.0f
#define POINT_COLUMN_X 1.4f

#define INDEXED_FILL_POS ((Vector3){FILL_COLUMN_X, INDEXED_ROW_Y, GRID_Z})
#define INDEXED_WIRE_POS ((Vector3){WIRE_COLUMN_X, INDEXED_ROW_Y, GRID_Z})
#define INDEXED_POINT_POS ((Vector3){POINT_COLUMN_X, INDEXED_ROW_Y, GRID_Z})

#define UNINDEXED_FILL_POS ((Vector3){FILL_COLUMN_X, UNINDEXED_ROW_Y, GRID_Z})
#define UNINDEXED_WIRE_POS ((Vector3){WIRE_COLUMN_X, UNINDEXED_ROW_Y, GRID_Z})
#define UNINDEXED_POINT_POS ((Vector3){POINT_COLUMN_X, UNINDEXED_ROW_Y, GRID_Z})

static const char* domain = "GLDC-MESH-DRAW-MODE";

#define CAMERA_FOVY_MIN 0.1f
#define CAMERA_FOVY_MAX 6.0f
#define CAMERA_FOVY_VELOCITY 6.0f
#define CAMERA_ORBIT_VELOCITY 2.0f
#define CAMERA_PITCH_MIN 0.1f
#define CAMERA_PITCH_MAX 2.5f

static const GLfloat SCENE_AMBIENT[4] = {0.32f, 0.32f, 0.32f, 1.0f};
static const GLfloat LIGHT_POS[4] = {1.330f, 1.345f, -1.418f, 1.0f};
static const GLfloat STATIC_AMBIENT[4] = {0.08f, 0.08f, 0.08f, 1.0f};
static const GLfloat STATIC_DIFFUSE[4] = {0.60f, 0.60f, 0.60f, 1.0f};
static const GLfloat WHITE4[4] = {1.0f, 1.0f, 1.0f, 1.0f};
static const GLfloat BLACK4[4] = {0.0f, 0.0f, 0.0f, 1.0f};

static void update_camera_orbit(Camera3D* camera, float dt);
static void fill_mesh_colors_yo(Mesh* mesh);

int main(void) {
    // SetTraceLogLevel(LOG_WARNING);
    InitWindow(640, 480, domain);

    Camera3D camera = {
        .position = (Vector3){0.0f, 0.0f, -3.0f},
        .target = (Vector3){0.0f, 0.0f, 0.0f},
        .up = Y_AXIS,
        .fovy = 3.111f,
        .projection = CAMERA_ORTHOGRAPHIC,
    };

    Mesh indexed_mesh = GenMeshCube(0.75f, 0.75f, 0.75f);
    Mesh unindexed_mesh = GenMeshSphere(0.5f, 16, 16);
    fill_mesh_colors_yo(&indexed_mesh);
    fill_mesh_colors_yo(&unindexed_mesh);

    Model indexed_model = LoadModelFromMesh(indexed_mesh);
    Model unindexed_model = LoadModelFromMesh(unindexed_mesh);

    float mesh_rotation = 0.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        GLfloat dynamic_ambient[4] = {0};
        GLfloat dynamic_diffuse[4] = {0};
        float pulse = 0.5f + 0.5f * sinf((float)GetTime() * 2.7f);
        float ambient_strength = 0.08f + pulse * 0.03f;
        float diffuse_strength = 0.60f + pulse * 0.60f;

        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        dynamic_ambient[0] = ambient_strength;
        dynamic_ambient[1] = ambient_strength;
        dynamic_ambient[2] = ambient_strength;
        dynamic_ambient[3] = 1.0f;

        dynamic_diffuse[0] = diffuse_strength;
        dynamic_diffuse[1] = diffuse_strength;
        dynamic_diffuse[2] = diffuse_strength;
        dynamic_diffuse[3] = 1.0f;

        mesh_rotation += -20.0f * GetFrameTime();
        update_camera_orbit(&camera, GetFrameTime());

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);
        rlSetLineWidth(1.0f);
        rlSetPointSize(3.0f);

        // ORIGINAL FILL / WIRE / POINT DRAW TESTS
        // unsigned char* indexed_saved_colors = indexed_model.meshes[0].colors;
        // DrawModelEx(indexed_model, INDEXED_FILL_POS, Y_AXIS, mesh_rotation, DEFAULT_SCALE, WHITE);
        // indexed_model.meshes[0].colors = NULL;
        // DrawModelWiresEx(indexed_model, INDEXED_WIRE_POS, Y_AXIS, mesh_rotation, DEFAULT_SCALE, BLUE);
        // DrawModelPointsEx(indexed_model, INDEXED_POINT_POS, Y_AXIS, mesh_rotation, DEFAULT_SCALE, MAGENTA);
        // indexed_model.meshes[0].colors = indexed_saved_colors;
        //
        // unsigned char* unindexed_saved_colors = unindexed_model.meshes[0].colors;
        // DrawModelEx(unindexed_model, UNINDEXED_FILL_POS, Y_AXIS, mesh_rotation, DEFAULT_SCALE, WHITE);
        // unindexed_model.meshes[0].colors = NULL;
        // DrawModelWiresEx(unindexed_model, UNINDEXED_WIRE_POS, Y_AXIS, mesh_rotation, DEFAULT_SCALE, BLUE);
        // DrawModelPointsEx(unindexed_model, UNINDEXED_POINT_POS, Y_AXIS, mesh_rotation, DEFAULT_SCALE, MAGENTA);
        // unindexed_model.meshes[0].colors = unindexed_saved_colors;

        // LEFT COLUMN: CONTROL_STATIC_STATIC
        // Expected: top cube and bottom sphere should both show the stable baseline lit colors.
        glShadeModel(GL_SMOOTH);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, SCENE_AMBIENT);
        glLightfv(GL_LIGHT0, GL_POSITION, LIGHT_POS);
        glLightfv(GL_LIGHT0, GL_AMBIENT, STATIC_AMBIENT);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, STATIC_DIFFUSE);
        DrawModelEx(indexed_model, INDEXED_FILL_POS, Y_AXIS, mesh_rotation, DEFAULT_SCALE, WHITE);

        glShadeModel(GL_FLAT);
        glLightfv(GL_LIGHT0, GL_AMBIENT, STATIC_AMBIENT);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, STATIC_DIFFUSE);
        DrawModelEx(unindexed_model, UNINDEXED_FILL_POS, Y_AXIS, mesh_rotation, DEFAULT_SCALE, WHITE);

        // MIDDLE COLUMN: REPRO_DYNAMIC_THEN_STATIC
        // Expected: if the bug reproduces, the bottom sphere here picks up the teal / washed tint.
        glShadeModel(GL_SMOOTH);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, SCENE_AMBIENT);
        glLightfv(GL_LIGHT0, GL_POSITION, LIGHT_POS);
        glLightfv(GL_LIGHT0, GL_AMBIENT, dynamic_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, dynamic_diffuse);
        DrawModelEx(indexed_model, INDEXED_WIRE_POS, Y_AXIS, mesh_rotation, DEFAULT_SCALE, WHITE);

        glShadeModel(GL_FLAT);
        glLightfv(GL_LIGHT0, GL_AMBIENT, STATIC_AMBIENT);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, STATIC_DIFFUSE);
        DrawModelEx(unindexed_model, UNINDEXED_WIRE_POS, Y_AXIS, mesh_rotation, DEFAULT_SCALE, WHITE);

        // RIGHT COLUMN: SEPARATE_LIGHTS
        // Expected: if LIGHT0/LIGHT1 isolation works, the bottom sphere here should keep the baseline look.
        glShadeModel(GL_SMOOTH);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, SCENE_AMBIENT);

        glLightfv(GL_LIGHT0, GL_POSITION, LIGHT_POS);
        glLightfv(GL_LIGHT0, GL_AMBIENT, dynamic_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, dynamic_diffuse);

        glLightfv(GL_LIGHT1, GL_POSITION, LIGHT_POS);
        glLightfv(GL_LIGHT1, GL_AMBIENT, STATIC_AMBIENT);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, STATIC_DIFFUSE);

        glDisable(GL_LIGHT1);
        DrawModelEx(indexed_model, INDEXED_POINT_POS, Y_AXIS, mesh_rotation, DEFAULT_SCALE, WHITE);

        glShadeModel(GL_FLAT);
        glDisable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        DrawModelEx(unindexed_model, UNINDEXED_POINT_POS, Y_AXIS, mesh_rotation, DEFAULT_SCALE, WHITE);

        // OPTIONAL SINGLE-SCENARIO TEST: REARM_MATERIAL
        // Comment out the three blocks above and uncomment only this block to isolate it.
        // Expected: if stale material tracking is the issue, this should reduce or remove the teal wash.
        // glShadeModel(GL_SMOOTH);
        // glEnable(GL_COLOR_MATERIAL);
        // glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        // glEnable(GL_LIGHTING);
        // glEnable(GL_LIGHT0);
        // glDisable(GL_LIGHT1);
        // glLightModelfv(GL_LIGHT_MODEL_AMBIENT, SCENE_AMBIENT);
        // glLightfv(GL_LIGHT0, GL_POSITION, LIGHT_POS);
        // glLightfv(GL_LIGHT0, GL_AMBIENT, dynamic_ambient);
        // glLightfv(GL_LIGHT0, GL_DIFFUSE, dynamic_diffuse);
        // DrawModelEx(indexed_model, INDEXED_FILL_POS, Y_AXIS, mesh_rotation, DEFAULT_SCALE, WHITE);

        // glDisable(GL_COLOR_MATERIAL);
        // glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, WHITE4);
        // glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, WHITE4);
        // glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, BLACK4);
        // glColor4ub(255, 255, 255, 255);
        // glEnable(GL_COLOR_MATERIAL);
        // glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

        // glShadeModel(GL_FLAT);
        // glLightfv(GL_LIGHT0, GL_AMBIENT, STATIC_AMBIENT);
        // glLightfv(GL_LIGHT0, GL_DIFFUSE, STATIC_DIFFUSE);
        // DrawModelEx(unindexed_model, UNINDEXED_FILL_POS, Y_AXIS, mesh_rotation, DEFAULT_SCALE, WHITE);

        // glShadeModel(GL_SMOOTH);
        // glDisable(GL_LIGHT0);
        // glDisable(GL_LIGHT1);
        // glDisable(GL_LIGHTING);
        // glDisable(GL_COLOR_MATERIAL);

        EndMode3D();
        EndDrawing();
    }

    UnloadModel(indexed_model);
    UnloadModel(unindexed_model);
    CloseWindow();
    return 0;
}

static void fill_mesh_colors_yo(Mesh* mesh) {
    if (!mesh->colors)
        mesh->colors = RL_CALLOC(mesh->vertexCount, sizeof(Color));
    Color* colors = (Color*)mesh->colors;
    Vector3* vertices = (Vector3*)mesh->vertices;
    BoundingBox bounds = GetMeshBoundingBox(*mesh);

    for (int i = 0; i < mesh->vertexCount; ++i) {
        Vector3 vertex = vertices[i];
        float hx = 0.5f * (bounds.max.x - bounds.min.x);
        float hy = 0.5f * (bounds.max.y - bounds.min.y);
        float hz = 0.5f * (bounds.max.z - bounds.min.z);
        float cx = 0.5f * (bounds.min.x + bounds.max.x);
        float cy = 0.5f * (bounds.min.y + bounds.max.y);
        float cz = 0.5f * (bounds.min.z + bounds.max.z);
        float nx = (hx > 0.0f) ? (vertex.x - cx) / hx : 0.0f;
        float ny = (hy > 0.0f) ? (vertex.y - cy) / hy : 0.0f;
        float nz = (hz > 0.0f) ? (vertex.z - cz) / hz : 0.0f;
        float len = sqrtf(nx * nx + ny * ny + nz * nz);
        if (len <= 0.0f)
            len = 1.0f;
        colors[i] = (Color){(unsigned char)lrintf(127.5f * (nx / len + 1.0f)),
                            (unsigned char)lrintf(127.5f * (ny / len + 1.0f)),
                            (unsigned char)lrintf(127.5f * (nz / len + 1.0f)),
                            255};
    }
}

static void update_camera_orbit(Camera3D* camera, float dt) {
    Vector3 dist_from_target = Vector3Subtract(camera->position, camera->target);
    float orbit_radius = Vector3Length(dist_from_target);
    float yaw = atan2f(dist_from_target.z, dist_from_target.x);
    float ground_radius = sqrtf(dist_from_target.x * dist_from_target.x + dist_from_target.z * dist_from_target.z);
    float pitch = atan2f(dist_from_target.y, ground_radius);
    float fovy = camera->fovy;

    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT))
        yaw += CAMERA_ORBIT_VELOCITY * dt;
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT))
        yaw -= CAMERA_ORBIT_VELOCITY * dt;
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP))
        pitch += CAMERA_ORBIT_VELOCITY * dt;
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN))
        pitch -= CAMERA_ORBIT_VELOCITY * dt;
    if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) > 0.0f)
        fovy -= GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) * CAMERA_FOVY_VELOCITY * dt;
    if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_TRIGGER) > 0.0f)
        fovy += GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_TRIGGER) * CAMERA_FOVY_VELOCITY * dt;

    pitch = Clamp(pitch, CAMERA_PITCH_MIN, CAMERA_PITCH_MAX);
    fovy = Clamp(fovy, CAMERA_FOVY_MIN, CAMERA_FOVY_MAX);
    camera->position.x = camera->target.x + orbit_radius * cosf(pitch) * cosf(yaw);
    camera->position.y = camera->target.y + orbit_radius * sinf(pitch);
    camera->position.z = camera->target.z + orbit_radius * cosf(pitch) * sinf(yaw);
    camera->fovy = fovy;
}
