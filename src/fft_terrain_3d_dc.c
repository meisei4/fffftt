#define FFFFTT_PROFILE_FFT_TERRAIN_3D
#include "fffftt.h"
#include <GL/gl.h>

#define LINE_WIDTH_RASTER_PIXELS 1.0f
#define POINT_SIZE_RASTER_PIXELS 3.0f
static const char* domain = "FFT-TERRAIN-3D-DC";

static Mesh mesh_a = {0};
static Mesh mesh_b = {0};
static Mesh flat_mesh = {0};

static Model model_a = {0};
static Model model_b = {0};
static Model flat_model = {0};

static float vertices[MESH_VERTEX_COUNT * 3] = {0};
static float normals[MESH_VERTEX_COUNT * 3] = {0};
static float texcoords[MESH_VERTEX_COUNT * 2] = {0};
static Color colors[MESH_VERTEX_COUNT] = {0};
static Color chroma_colors[MESH_VERTEX_COUNT] = {0};

static float flat_vertices[FLAT_VERTEX_COUNT * 3] = {0};
static float flat_normals[FLAT_VERTEX_COUNT * 3] = {0};
static Color flat_colors[FLAT_VERTEX_COUNT] = {0};
static unsigned char lane_chroma_id[LANE_COUNT] = {0};
static float chroma_mask_field[LANE_COUNT][LANE_POINT_COUNT] = {0};

static int inspection_ready = 0;
static int cur_frame_pos = 0;
static int retention_window_min_frame_pos = 0;
static int retention_window_max_frame_pos = 0;
static void update_playback_controls_fft(void);
static void consume_current_fft_frame(void);
static void build_fft_terrain_lane_from_frame_pos(int lane, int frame_pos);
static void inspection_step(int dir);
static void update_fft_terrain_meshes(void);
static void rebase_fft_history(void);

int main(void) {
    // SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    font = LoadFont(RD_FONT);
    fft_data.tapback_pos = ANALYSIS_TAPBACK_POS_DEFAULT;
    fft_data.work_buffer = RL_CALLOC(ANALYSIS_WINDOW_SIZE_IN_FRAMES, sizeof(FFTComplex));
    fft_data.smoothed_spectrum_magnitudes = RL_CALLOC(ANALYSIS_SPECTRUM_BIN_COUNT, sizeof(float));
    fft_data.raw_spectrum_magnitudes = RL_CALLOC(ANALYSIS_FFT_HISTORY_FRAME_COUNT, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]));
    fft_data.spectrum_levels = RL_CALLOC(ANALYSIS_FFT_HISTORY_FRAME_COUNT, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]));

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
    set_audio_track(SHADERTOY_EXPERIMENT);
    audio_stream = LoadAudioStream(SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    PlayAudioStream(audio_stream);

    Camera3D camera = {
        .position = (Vector3){1.45625f * 3.0f, 1.345f * 3.0f, -1.36625f * 3.0f},
        .target = (Vector3){0.0f, 1.5f, 0.0f},
        .up = Y_AXIS,
        .fovy = 8.0f,
        .projection = CAMERA_ORTHOGRAPHIC,
    };

    mesh_a = GenMeshPlane(1.0f, 1.0f, LANE_POINT_COUNT - 1, LANE_COUNT - 1);
    // NOTE: FOR USING CUSTOM LANE TOPOLOGY APPROACH WITH "WIRE DRAWS":
    // mesh_a.triangleCount = LINE_SEGMENT_COUNT; // TODO: defeats the purpose of genMesh...
    // mesh_a.indices = RL_CALLOC(LINE_INDEX_COUNT, sizeof(unsigned short)); // TODO: defeats the purpose of genMesh...
    // fill_mesh_indices_lane_topology(mesh_a.indices);

    update_mesh_vertices(mesh_a.vertices, &lane_point_values[0][0], LANE_POINT_COUNT, AMPLITUDE_Y_SCALE);
    mesh_a.colors = RL_CALLOC(mesh_a.vertexCount, sizeof(Color));

    Texture2D lane_mask_texture = build_lane_mask(texcoords, LANE_POINT_COUNT);
    wave_cursor_texture = build_lane_mask_glow(texcoords, LANE_POINT_COUNT);

    model_a = LoadModelFromMesh(mesh_a);
    unsigned char* saved_colors = model_a.meshes[0].colors;
    model_a.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = lane_mask_texture;
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
    // update_mesh_colors_jet_scroll(colors, LANE_POINT_COUNT, wave_cursor);
    update_fft_terrain_meshes();
    update_mesh_normals_smooth(normals, vertices, LANE_POINT_COUNT);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        update_audio_track_cycle();
        update_playback_controls_fft();

        int audio_dirty = 0;
        while (fffftt_audio_process(chunk_samples)) {
            apply_blackman_window();
            shz_fft((shz_complex_t*)fft_data.work_buffer, (size_t)ANALYSIS_WINDOW_SIZE_IN_FRAMES);
            build_spectrum();
            consume_current_fft_frame();
            audio_dirty = 1;
        }

        if (audio_dirty) {
            update_fft_terrain_meshes();
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

        // NOTE: FOR USING CUSTOM LANE TOPOLOGY APPROACH WITH "WIRE DRAWS":
        // rlEnableStatePointer(GL_COLOR_ARRAY, mesh_a.colors);
        // rlEnableStatePointer(GL_VERTEX_ARRAY, mesh_a.vertices);
        // glDrawElements(RL_LINES, mesh_a.triangleCount * 2, GL_UNSIGNED_SHORT, (const unsigned short*)mesh_a.indices);
        // rlDisableStatePointer(GL_COLOR_ARRAY);
        // rlDisableStatePointer(GL_VERTEX_ARRAY);

        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (const GLfloat[]){0.2f, 0.2f, 0.2f, 1.0f});
        glLightfv(GL_LIGHT0, GL_AMBIENT, (const GLfloat[]){0.0f, 0.0f, 0.0f, 1.0f});
        // glLightfv(GL_LIGHT0, GL_DIFFUSE, (const GLfloat[]){0.6f, 0.6f, 0.6f, 1.0f});
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
        glLightfv(GL_LIGHT0, GL_POSITION, (const GLfloat[]){light0_pos.x, light0_pos.y, light0_pos.z, 1.0f});
        rlDisableBackfaceCulling();
        DrawModelEx(model_a, MIDDLE, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);
        rlEnableBackfaceCulling();
        glDisable(GL_LIGHTING);
        draw_lantern(light0_pos);
        draw_lantern_glow(light0_pos);

        glEnable(GL_LIGHTING);
        glShadeModel(GL_FLAT);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (const GLfloat[]){0.0f, 0.0f, 0.0f, 1.0f});
        glLightfv(GL_LIGHT0, GL_AMBIENT, (const GLfloat[]){0.0f, 0.0f, 0.0f, 1.0f});
        glLightfv(GL_LIGHT0, GL_DIFFUSE, (const GLfloat[]){1.0f, 1.0f, 1.0f, 1.0f});
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, (const GLfloat[]){0.0f, 0.0f, 0.0f, 0.0f});
        rlDisableBackfaceCulling();
        DrawModelEx(flat_model, TOP, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE); //TODO: too powerful...
        rlEnableBackfaceCulling();
        glShadeModel(GL_SMOOTH);
        glDisable(GL_LIGHTING);

        // DrawModelEx(model_b, MIDDLE, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);

        model_a.meshes[0].colors = NULL;
        // model_a.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = saved_texture_id;
        // rlDisableDepthTest();
        // rlDisableBackfaceCulling();
        // DrawModelEx(model_a, MIDDLE, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);
        // rlEnableBackfaceCulling();
        // rlEnableDepthTest();
        // model_a.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = 0;

        rlEnablePointMode();
        rlDisableDepthTest();
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
    RL_FREE(fft_data.raw_spectrum_magnitudes);
    RL_FREE(fft_data.spectrum_levels);
    RL_FREE(fft_data.smoothed_spectrum_magnitudes);
    RL_FREE(fft_data.work_buffer);
    UnloadFont(font);
    CloseWindow();
    return 0;
}

static void update_playback_controls_fft(void) {
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
            rebase_fft_history();
            update_fft_terrain_meshes();
        } else {
            is_paused = false;
            inspection_ready = 0;
            PlayAudioStream(audio_stream);
            while (fffftt_audio_process(resume_chunk_samples)) {
                apply_blackman_window();
                shz_fft((shz_complex_t*)fft_data.work_buffer, (size_t)ANALYSIS_WINDOW_SIZE_IN_FRAMES);
                build_spectrum();
                consume_current_fft_frame();
                analysis_dirty = 1;
            }
        }
    }

    if (is_paused && sticky_nav(GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
        wave_cursor = WRAP_MINUS(wave_cursor, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES, wave.frameCount);
        seek_delta_chunks--;
        int target_frame_pos = cur_frame_pos - 1;
        if (inspection_ready && cur_frame_pos > 0 && target_frame_pos >= retention_window_min_frame_pos + (LANE_COUNT - 1)) {
            cur_frame_pos = target_frame_pos;
            inspection_step(BACKWARD);
        } else {
            rebase_fft_history();
        }
        onset_gate = onset_gate_history[WRAP(cur_frame_pos, ANALYSIS_FFT_HISTORY_FRAME_COUNT)];
        update_fft_terrain_meshes();
    } else if (is_paused && sticky_nav(GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
        wave_cursor = WRAP_PLUS(wave_cursor, AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES, wave.frameCount);
        seek_delta_chunks++;
        int target_frame_pos = cur_frame_pos + 1;
        if (inspection_ready && target_frame_pos <= retention_window_max_frame_pos) {
            cur_frame_pos = target_frame_pos;
            inspection_step(FORWARD);
        } else if (inspection_ready && cur_frame_pos == retention_window_max_frame_pos) {
            fffftt_inspection_fill_analysis_window(wave_cursor);

            apply_blackman_window();
            shz_fft((shz_complex_t*)fft_data.work_buffer, (size_t)ANALYSIS_WINDOW_SIZE_IN_FRAMES);
            build_spectrum();
            int frame_pos = fft_data.frame_pos - 1;
            update_onset_gate_fft(&fft_data, 0);

            retention_window_max_frame_pos = frame_pos;
            if (retention_window_max_frame_pos - retention_window_min_frame_pos >= ANALYSIS_FFT_HISTORY_FRAME_COUNT) {
                retention_window_min_frame_pos = retention_window_max_frame_pos - (ANALYSIS_FFT_HISTORY_FRAME_COUNT - 1);
            }
            cur_frame_pos = retention_window_max_frame_pos;
            inspection_step(FORWARD);
        } else {
            rebase_fft_history();
        }
        onset_gate = onset_gate_history[WRAP(cur_frame_pos, ANALYSIS_FFT_HISTORY_FRAME_COUNT)];
        update_fft_terrain_meshes();
    }

    if (analysis_dirty) {
        update_fft_terrain_meshes();
    }
}

static void consume_current_fft_frame(void) {
    int cur_history_frame_pos = WRAP_MINUS(fft_data.history_frame_pos, 1, ANALYSIS_FFT_HISTORY_FRAME_COUNT);
    float* spectrum_levels = fft_data.spectrum_levels[cur_history_frame_pos];
    float* raw_spectrum_magnitudes = fft_data.raw_spectrum_magnitudes[cur_history_frame_pos];
    advance_lane_history(&lane_point_values[0][0], LANE_POINT_COUNT);
    advance_lane_history_u8(lane_chroma_id, 1);
    advance_lane_history(&chroma_mask_field[0][0], LANE_POINT_COUNT);
    for (int point_index = 0; point_index < LANE_POINT_COUNT; point_index++) {
        lane_point_values[0][point_index] = spectrum_levels[(point_index * (ANALYSIS_SPECTRUM_BIN_COUNT - 1)) / (LANE_POINT_COUNT - 1)];
    }
    build_chroma_fields(
        &lane_chroma_id[0], &chroma_mask_field[0][0], &lane_point_values[0][0], raw_spectrum_magnitudes, LANE_POINT_COUNT, 1, ANALYSIS_SPECTRUM_BIN_COUNT - 1);
    update_onset_gate_fft(&fft_data, 0);
}

static void build_fft_terrain_lane_from_frame_pos(int lane, int frame_pos) {
    if (frame_pos < retention_window_min_frame_pos || frame_pos > retention_window_max_frame_pos) {
        MEMSET(lane_point_values[lane], 0, sizeof(lane_point_values[lane]));
        lane_chroma_id[lane] = 0;
        MEMSET(chroma_mask_field[lane], 0, sizeof(chroma_mask_field[lane]));
        return;
    }

    int history_frame_pos = WRAP(frame_pos, ANALYSIS_FFT_HISTORY_FRAME_COUNT);
    float* spectrum_levels = fft_data.spectrum_levels[history_frame_pos];
    float* raw_spectrum_magnitudes = fft_data.raw_spectrum_magnitudes[history_frame_pos];
    for (int i = 0; i < LANE_POINT_COUNT; i++) {
        int bin = (i * (ANALYSIS_SPECTRUM_BIN_COUNT - 1)) / (LANE_POINT_COUNT - 1);
        lane_point_values[lane][i] = spectrum_levels[bin];
    }
    build_chroma_fields(&lane_chroma_id[lane],
                        &chroma_mask_field[lane][0],
                        &lane_point_values[lane][0],
                        raw_spectrum_magnitudes,
                        LANE_POINT_COUNT,
                        1,
                        ANALYSIS_SPECTRUM_BIN_COUNT - 1);
}

static void inspection_step(int dir) {
    for (int i = (dir == FORWARD) ? LANE_COUNT - 1 : 0; i != ((dir == FORWARD) ? 0 : LANE_COUNT - 1); i -= dir) {
        MEMCPY(lane_point_values[i], lane_point_values[i - dir], sizeof(lane_point_values[i]));
        lane_chroma_id[i] = lane_chroma_id[i - dir];
        MEMCPY(chroma_mask_field[i], chroma_mask_field[i - dir], sizeof(chroma_mask_field[i]));
    }

    build_fft_terrain_lane_from_frame_pos((dir == FORWARD) ? 0 : LANE_COUNT - 1, (dir == FORWARD) ? cur_frame_pos : cur_frame_pos - (LANE_COUNT - 1));
}

static void update_fft_terrain_meshes(void) {
    update_mesh_vertices(vertices, &lane_point_values[0][0], LANE_POINT_COUNT, AMPLITUDE_Y_SCALE);
    // update_mesh_colors_jet_scroll(colors, LANE_POINT_COUNT, wave_cursor);
    update_mesh_colors_chroma(chroma_colors, lane_chroma_id, &chroma_mask_field[0][0], LANE_POINT_COUNT);
    update_mesh_normals_smooth(normals, vertices, LANE_POINT_COUNT);
    //NOTE: by pure accident i was forgetting to update the normals here. This lead me to an intersting finding... position lights actually shade faces based on:
    // 1. Normals themselves (ofc -- in this case static, and the result of `GenMeshPlane` filling everything as {0.0f, 1.0f, 0.0f})
    // 2. BUT ALSO "direction from vertex -> position light" **This will actually CHANGE as the vertex positions animate with the terrain!!!!**
    //    - what results with static upfacing normals and then animated/dynamic vertex positions is a very strange but cool effect of like a thundercloud where
    //    - the lightings actual source is very hard to see but in a way that emulates faint almost curtain like shading...
    /*
    update_mesh_normals_smooth(normals, vertices, LANE_POINT_COUNT);
    */

    expand_mesh_colors_flat(flat_colors, chroma_colors, mesh_a.indices, FLAT_VERTEX_COUNT);
    update_mesh_vertices_flat(flat_vertices, vertices, mesh_a.indices, FLAT_VERTEX_COUNT);
    update_mesh_normals_flat(flat_normals, flat_vertices, TERRAIN_TRIANGLE_COUNT);

    build_mesh_smooth(&mesh_a, vertices, normals, colors, texcoords, MESH_VERTEX_COUNT);
    build_mesh_smooth(&mesh_b, vertices, normals, chroma_colors, mesh_b.texcoords, MESH_VERTEX_COUNT);
    build_mesh_flat(&flat_mesh, flat_vertices, flat_normals, flat_colors, FLAT_VERTEX_COUNT);
}

static void rebase_fft_history(void) {
    inspection_ready = 0;
    MEMSET(fft_data.smoothed_spectrum_magnitudes, 0, sizeof(float) * ANALYSIS_SPECTRUM_BIN_COUNT);
    MEMSET(fft_data.raw_spectrum_magnitudes, 0, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]) * ANALYSIS_FFT_HISTORY_FRAME_COUNT);
    MEMSET(fft_data.spectrum_levels, 0, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]) * ANALYSIS_FFT_HISTORY_FRAME_COUNT);
    MEMSET(lane_point_values, 0, sizeof(lane_point_values));
    MEMSET(lane_chroma_id, 0, sizeof(lane_chroma_id));
    MEMSET(chroma_mask_field, 0, sizeof(chroma_mask_field));
    fft_data.history_frame_pos = 0;
    fft_data.frame_pos = 0;
    reset_shared_adaptive_audio_state();

    for (int i = ANALYSIS_FFT_HISTORY_FRAME_COUNT - 1; i >= 0; i--) {
        int replay_frame_offset = WRAP(i * AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES, wave.frameCount);
        int chunk_start_frame = WRAP_MINUS(wave_cursor, replay_frame_offset, wave.frameCount);
        fffftt_inspection_fill_analysis_window(chunk_start_frame);
        apply_blackman_window();
        shz_fft((shz_complex_t*)fft_data.work_buffer, (size_t)ANALYSIS_WINDOW_SIZE_IN_FRAMES);
        build_spectrum();
        consume_current_fft_frame();
    }

    inspection_ready = 1;
    retention_window_max_frame_pos = fft_data.frame_pos - 1;
    retention_window_min_frame_pos = retention_window_max_frame_pos - (ANALYSIS_FFT_HISTORY_FRAME_COUNT - 1);
    cur_frame_pos = retention_window_max_frame_pos;
    onset_gate = onset_gate_history[WRAP(cur_frame_pos, ANALYSIS_FFT_HISTORY_FRAME_COUNT)];
}
