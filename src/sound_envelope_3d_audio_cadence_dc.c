#include "fffftt.h"
#include "rlgl.h"
#include <GL/gl.h>

static const char* domain = "SOUND-ENVELOPE-3D-AUDIO-CADENCE-DC";

#define LANE_COUNT 5
#define LANE_POINT_COUNT 64
#define WAVEFORM_SAMPLES_PER_LANE_POINT (WINDOW_SIZE / LANE_POINT_COUNT)

#define AMPLITUDE_Y_SCALE 2.0f //TODO: manually tuned, not even close to exact parity
#define FRONT_LANE_SMOOTHING 0.4f
#define ENVELOPE_HALF_SPAN 0.5f
#define ENVELOPE_LINE_WIDTH_RASTER_PIXELS 2.0f
#define LINE_LENGTH_SCALE 1.75f //TODO: manually tuned alignment...

static Vector3 envelope_mesh_vertices[LANE_COUNT][LANE_POINT_COUNT] = {0};
static float lane_point_samples[LANE_COUNT][LANE_POINT_COUNT] = {0};
static float waveform_window_samples[WINDOW_SIZE] = {0};

static void advance_envelope_history(void);
static void update_envelope_mesh_vertices(void);
static void update_camera_orbit(Camera3D* camera, float dt);

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
        .position = (Vector3){-1.093f, 1.126f, 1.165f}, //TODO: manually tuned alignment...
        .target = (Vector3){0.0f, 0.25f, 0.0f},
        .up = (Vector3){0.0f, 1.0f, 0.0f},
        .fovy = 3.111f, //TODO: manually tuned alignment... 2D software isometric projection -> true 3D orthographic projection is tough
        .projection = CAMERA_ORTHOGRAPHIC,
    };

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

            advance_envelope_history();
        }

        update_envelope_mesh_vertices();
        update_camera_orbit(&camera, (float)GetFrameTime());

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);

        rlSetLineWidth(ENVELOPE_LINE_WIDTH_RASTER_PIXELS);
        for (int i = 0; i < LANE_COUNT; i++) {
            rlEnableStatePointer(GL_VERTEX_ARRAY, envelope_mesh_vertices[i]);
            rlDrawVertexArrayCustom(0, LANE_POINT_COUNT, GL_LINE_STRIP);
        }

        EndMode3D();
        EndDrawing();
    }

    UnloadAudioStream(audio_stream);
    UnloadWave(wav);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

static void advance_envelope_history(void) {
    for (int i = LANE_COUNT - 1; i > 0; i--) {
        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            lane_point_samples[i][j] = lane_point_samples[i - 1][j];
        }
    }

    for (int i = 0; i < LANE_POINT_COUNT; i++) {
        float amplitude_sum = 0.0f;
        int lane_window_offset = i * WAVEFORM_SAMPLES_PER_LANE_POINT;

        for (int j = 0; j < WAVEFORM_SAMPLES_PER_LANE_POINT; j++) {
            amplitude_sum += fabsf(waveform_window_samples[lane_window_offset + j]);
        }

        lane_point_samples[0][i] = (amplitude_sum / (float)WAVEFORM_SAMPLES_PER_LANE_POINT) * FRONT_LANE_SMOOTHING;
    }
}

static void update_envelope_mesh_vertices(void) {
    for (int i = 0; i < LANE_COUNT; i++) {
        float z = ENVELOPE_HALF_SPAN - ((float)i / (float)(LANE_COUNT - 1));
        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            float x = (((float)j / (float)(LANE_POINT_COUNT - 1)) - ENVELOPE_HALF_SPAN) * LINE_LENGTH_SCALE;
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
