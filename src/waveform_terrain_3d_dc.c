#define FFFFTT_PROFILE_WAVEFORM_TERRAIN_3D
#include "fffftt.h"
#include "raylib.h"
#include "rlgl.h"
#include <GL/gl.h>

#define LINE_WIDTH_RASTER_PIXELS 1.0f
#define POINT_SIZE_RASTER_PIXELS 3.0f

static const char* domain = "WAVEFORM-TERRAIN-3D-DC";

static Mesh mesh = {0};
static float lane_point_samples[LANE_COUNT][LANE_POINT_COUNT] = {0};
static float waveform_window_samples[WINDOW_SIZE] = {0};

int main(void) {
    int16_t chunk_samples[AUDIO_STREAM_RING_BUFFER_SIZE] = {0};

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_STREAM_RING_BUFFER_SIZE);
    Wave wav = LoadWave(RD_SHADERTOY_EXPERIMENT_22K_WAV);
    WaveFormat(&wav, SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    AudioStream audio_stream = LoadAudioStream(SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    PlayAudioStream(audio_stream);

    size_t wav_cursor = 0;
    int16_t* wav_pcm16 = (int16_t*)wav.data;

    Camera3D camera = {
        .position = (Vector3){1.45625f, 1.345f, -1.36625f},
        .target = (Vector3){0.0f, 0.25f, 0.0f},
        .up = (Vector3){0.0f, 1.0f, 0.0f},
        .fovy = 3.111f,
        .projection = CAMERA_ORTHOGRAPHIC,
    };

    mesh = GenMeshPlane(1.0f, 1.0f, LANE_POINT_COUNT - 1, LANE_COUNT - 1);
    mesh.colors = RL_CALLOC(mesh.vertexCount, sizeof(Color));

    update_terrain_mesh_vertices(mesh.vertices, &lane_point_samples[0][0]);
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
            for (int i = 0; i < AUDIO_STREAM_RING_BUFFER_SIZE; i++) {
                chunk_samples[i] = wav_pcm16[wav_cursor];
                if (++wav_cursor >= wav.frameCount) {
                    wav_cursor = 0;
                }
            }

            UpdateAudioStream(audio_stream, chunk_samples, AUDIO_STREAM_RING_BUFFER_SIZE);

            for (int i = 0; i < WINDOW_SIZE; i++) {
                waveform_window_samples[i] = (float)chunk_samples[WINDOW_SIZE + i] / PCM_SAMPLE_MAX_F;
            }

            advance_lane_history(&lane_point_samples[0][0]);
            for (int i = 0; i < LANE_POINT_COUNT; i++) {
                lane_point_samples[0][i] = waveform_window_samples[(i * (WINDOW_SIZE - 1)) / (LANE_POINT_COUNT - 1)];
            }
        }

        update_terrain_mesh_vertices(mesh.vertices, &lane_point_samples[0][0]);
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
    UnloadWave(wav);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
