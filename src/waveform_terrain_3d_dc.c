#define FFFFTT_PROFILE_WAVEFORM_TERRAIN_3D
#include "fffftt.h"
#include "raylib.h"
#include "rlgl.h"
#include <GL/gl.h>

#define LINE_WIDTH_RASTER_PIXELS 1.0f
#define POINT_SIZE_RASTER_PIXELS 3.0f

static const char* domain = "WAVEFORM-TERRAIN-3D-DC";

static Mesh mesh = {0};
static float lane_point_values[LANE_COUNT][LANE_POINT_COUNT] = {0};
static float analysis_window_samples[ANALYSIS_WINDOW_SIZE_IN_FRAMES] = {0};

int main(void) {
    int16_t chunk_samples[AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES] = {0};

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
    Wave wave = LoadWave(RD_SHADERTOY_EXPERIMENT_22K_WAV);
    WaveFormat(&wave, SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    AudioStream audio_stream = LoadAudioStream(SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    PlayAudioStream(audio_stream);

    size_t wave_cursor = 0;
    int16_t* wave_pcm16 = (int16_t*)wave.data;

    Camera3D camera = {
        .position = (Vector3){1.45625f, 1.345f, -1.36625f},
        .target = (Vector3){0.0f, 0.25f, 0.0f},
        .up = (Vector3){0.0f, 1.0f, 0.0f},
        .fovy = 3.111f,
        .projection = CAMERA_ORTHOGRAPHIC,
    };

    mesh = GenMeshPlane(1.0f, 1.0f, LANE_POINT_COUNT - 1, LANE_COUNT - 1);
    mesh.colors = RL_CALLOC(mesh.vertexCount, sizeof(Color));

    update_terrain_mesh_vertices(mesh.vertices, &lane_point_values[0][0]);
    // fill_mesh_indices_lane_topology(mesh.indices); //TODO: this would be for a direct drawElements call not a mesh draw, custom topology is a bit complicated
    fill_mesh_colors((Color*)mesh.colors);

    Model model = LoadModelFromMesh(mesh);
    Mesh* draw_mesh = &model.meshes[0];

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
        update_camera_orbit(&camera, GetFrameTime());

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);
        rlSetLineWidth(LINE_WIDTH_RASTER_PIXELS);
        rlSetPointSize(POINT_SIZE_RASTER_PIXELS);

        DrawModelEx(model, (Vector3){0.0f}, (Vector3){0.0f, 1.0f, 0.0f}, 0.0f, (Vector3){1.0f, 1.0f, 1.0f}, WHITE);
        unsigned char* saved_colors = draw_mesh->colors;

        draw_mesh->colors = NULL;
        DrawModelWiresEx(model, (Vector3){0.0f}, (Vector3){0.0f, 1.0f, 0.0f}, 0.0f, (Vector3){1.0f, 1.0f, 1.0f}, BLUE);
        DrawModelPointsEx(model, (Vector3){0.0f}, (Vector3){0.0f, 1.0f, 0.0f}, 0.0f, (Vector3){1.0f, 1.0f, 1.0f}, MAGENTA);
        draw_mesh->colors = saved_colors;

        EndMode3D();
        // DrawFPS(540, 420);
        EndDrawing();
    }

    UnloadModel(model);
    UnloadAudioStream(audio_stream);
    UnloadWave(wave);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
