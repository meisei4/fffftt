#include "audio_spectrum_analyzer.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <GL/gl.h>
#include <math.h>
#include <stdint.h>

static const char* domain = "SOUND-ENVELOPE-3D-DC";

#define LANE_COUNT 5
#define LANE_POINT_COUNT 64
#define WAVEFORM_SAMPLES_PER_LANE_POINT (WAVEFORM_BUFFER_SIZE / LANE_POINT_COUNT)

#define AMPLITUDE_Y_SCALE 0.75f
#define ENVELOPE_HALF_SPAN 0.5f
#define ENVELOPE_LINE_WIDTH 2.0f

#define CAMERA_FOVY_MIN 0.1f
#define CAMERA_FOVY_MAX 6.0f
#define CAMERA_FOVY_VELOCITY 6.0f
#define CAMERA_ORBIT_VELOCITY 2.0f
#define CAMERA_PITCH_MIN 0.1f
#define CAMERA_PITCH_MAX 2.5f

static Vector3 envelope_mesh_vertices[LANE_COUNT][LANE_POINT_COUNT] = {0};
static float lane_point_samples[LANE_COUNT][LANE_POINT_COUNT] = {0};
static float waveform_window_samples[WAVEFORM_WINDOW_SIZE] = {0};

static void update_envelope_mesh_vertices(void);
static void update_camera_orbit(Camera3D* camera, float dt);

int main(void) {
    int16_t chunk_samples[WAVEFORM_AUDIO_STREAM_RING_BUFFER_SIZE] = {0};

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(WAVEFORM_AUDIO_STREAM_RING_BUFFER_SIZE);
    Wave wave = LoadWave("/rd/shadertoy_experiment_22050hz_pcm16_mono.wav");
    //Wave wave = LoadWave("/rd/hellion_one_fifth_22050hz_pcm16_mono.wav");
    WaveFormat(&wave, SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    AudioStream stream = LoadAudioStream(SAMPLE_RATE, PER_SAMPLE_BIT_DEPTH, MONO);
    PlayAudioStream(stream);

    size_t wave_cursor = 0;
    int16_t* pcm_data = (int16_t*)wave.data;

    Camera3D camera = {
        .position = (Vector3){-1.0f, 0.85f, 1.40f},
        .target = (Vector3){0.0f, 0.25f, 0.0f},
        .up = (Vector3){0.0f, 1.0f, 0.0f},
        .fovy = 2.0f,
        .projection = CAMERA_ORTHOGRAPHIC,
    };

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        while (IsAudioStreamProcessed(stream)) {
            for (int i = 0; i < WAVEFORM_AUDIO_STREAM_RING_BUFFER_SIZE; i++) {
                chunk_samples[i] = pcm_data[wave_cursor];
                if (++wave_cursor >= wave.frameCount) {
                    wave_cursor = 0;
                }
            }

            UpdateAudioStream(stream, chunk_samples, WAVEFORM_AUDIO_STREAM_RING_BUFFER_SIZE);

            for (int i = 0; i < WAVEFORM_WINDOW_SIZE; i++) {
                waveform_window_samples[i] = (float)chunk_samples[WAVEFORM_WINDOW_SIZE + i] / PCM_SAMPLE_MAX_F;
            }
        }

        update_envelope_mesh_vertices();
        update_camera_orbit(&camera, (float)GetFrameTime());

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);

        rlSetLineWidth(ENVELOPE_LINE_WIDTH);
        for (int lane_index = 0; lane_index < LANE_COUNT; lane_index++) {
            rlEnableStatePointer(GL_VERTEX_ARRAY, envelope_mesh_vertices[lane_index]);
            rlDrawVertexArrayCustom(0, LANE_POINT_COUNT, GL_LINE_STRIP);
        }

        EndMode3D();
        EndDrawing();
    }

    UnloadAudioStream(stream);
    UnloadWave(wave);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

static void update_envelope_mesh_vertices() {
    for (int i = LANE_COUNT - 1; i > 0; i--) {
        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            lane_point_samples[i][j] = lane_point_samples[i - 1][j];
        }
    }

    for (int i = 0; i < LANE_POINT_COUNT; i++) {
        float absolute_amplitude_sum = 0.0f;
        int waveform_sample_base = i * WAVEFORM_SAMPLES_PER_LANE_POINT;

        for (int j = 0; j < WAVEFORM_SAMPLES_PER_LANE_POINT; j++) {
            absolute_amplitude_sum += fabsf(waveform_window_samples[waveform_sample_base + j]);
        }

        lane_point_samples[0][i] = absolute_amplitude_sum / (float)WAVEFORM_SAMPLES_PER_LANE_POINT;
    }

    for (int i = 0; i < LANE_COUNT; i++) {
        float z = ENVELOPE_HALF_SPAN - ((float)i / (float)(LANE_COUNT - 1));
        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            float x = ((float)j / (float)(LANE_POINT_COUNT - 1)) - ENVELOPE_HALF_SPAN;
            float y = lane_point_samples[i][j] * AMPLITUDE_Y_SCALE;
            envelope_mesh_vertices[i][j].x = x;
            envelope_mesh_vertices[i][j].y = y;
            envelope_mesh_vertices[i][j].z = z;
        }
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
