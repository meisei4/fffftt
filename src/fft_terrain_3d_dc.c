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

static void update_playback_controls_fft(void);

static float vertices[MESH_VERTEX_COUNT * 3] = {0};
static float normals[MESH_VERTEX_COUNT * 3] = {0};
static Color colors[MESH_VERTEX_COUNT] = {0};
static Color pitch_class_colors[MESH_VERTEX_COUNT] = {0};

static float flat_vertices[FLAT_VERTEX_COUNT * 3] = {0};
static float flat_normals[FLAT_VERTEX_COUNT * 3] = {0};
static Color flat_colors[FLAT_VERTEX_COUNT] = {0};
static unsigned char chroma_index_field[LANE_COUNT][LANE_POINT_COUNT] = {0};
static float chroma_strength_field[LANE_COUNT][LANE_POINT_COUNT] = {0};

int main(void) {
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    font = LoadFont(RD_FONT);
    fft_data.tapback_pos = ANALYSIS_TAPBACK_POS_DEFAULT;
    fft_data.work_buffer = RL_CALLOC(ANALYSIS_WINDOW_SIZE_IN_FRAMES, sizeof(FFTComplex));
    fft_data.prev_spectrum_bin_levels = RL_CALLOC(ANALYSIS_SPECTRUM_BIN_COUNT, sizeof(float));
    fft_data.spectrum_history_levels = RL_CALLOC(ANALYSIS_FFT_HISTORY_FRAME_COUNT, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]));

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

    update_mesh_vertices(mesh_a.vertices, &lane_point_values[0][0], LANE_POINT_COUNT);
    mesh_a.colors = RL_CALLOC(mesh_a.vertexCount, sizeof(Color));

    Texture2D lane_mask_texture = build_lane_mask(mesh_a.texcoords, LANE_POINT_COUNT);
    wave_cursor_texture = build_lane_mask_glow(mesh_a.texcoords, LANE_POINT_COUNT);
    // Texture2D lane_mask_texture = build_lane_mask_glow(mesh_a.texcoords);

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

    update_mesh_vertices(vertices, &lane_point_values[0][0], LANE_POINT_COUNT);
    update_mesh_normals_smooth(normals, vertices, LANE_POINT_COUNT);
    update_mesh_colors_pitch_class(pitch_class_colors, &chroma_index_field[0][0], &chroma_strength_field[0][0], LANE_POINT_COUNT);
    expand_mesh_colors_flat(flat_colors, pitch_class_colors, mesh_a.indices);

    update_mesh_vertices_flat(flat_vertices, vertices, mesh_a.indices);
    update_mesh_normals_flat(flat_normals, flat_vertices);

    build_mesh_smooth(&mesh_a, vertices, normals, colors, MESH_VERTEX_COUNT);
    build_mesh_smooth(&mesh_b, vertices, normals, pitch_class_colors, MESH_VERTEX_COUNT);
    build_mesh_flat(&flat_mesh, flat_vertices, flat_normals, flat_colors);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        //TODO: this is not the correct approach, it causes way too much churn to rebuild the envelope, should just figure out a compromise
        update_playback_controls_fft();

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

            apply_blackman_window();
            shz_fft((shz_complex_t*)fft_data.work_buffer, (size_t)ANALYSIS_WINDOW_SIZE_IN_FRAMES);
            build_spectrum();

            float* spectrum_bin_levels =
                fft_data.spectrum_history_levels[(fft_data.history_pos - 1 + ANALYSIS_FFT_HISTORY_FRAME_COUNT) % ANALYSIS_FFT_HISTORY_FRAME_COUNT];
            advance_lane_history(&lane_point_values[0][0], LANE_POINT_COUNT);
            advance_lane_history_u8(&chroma_index_field[0][0], LANE_POINT_COUNT);
            advance_lane_history(&chroma_strength_field[0][0], LANE_POINT_COUNT);
            for (int i = 0; i < LANE_POINT_COUNT; i++) {
                lane_point_values[0][i] = spectrum_bin_levels[(i * (ANALYSIS_SPECTRUM_BIN_COUNT - 1)) / (LANE_POINT_COUNT - 1)];
            }
            build_pitch_class_color_field(&chroma_index_field[0][0],
                                          &chroma_strength_field[0][0],
                                          &lane_point_values[0][0],
                                          spectrum_bin_levels,
                                          LANE_POINT_COUNT,
                                          1,
                                          ANALYSIS_SPECTRUM_BIN_COUNT - 1);
            update_onset_interpolation_factor_fft(&fft_data);
            audio_dirty = 1;
        }

        if (audio_dirty) {
            update_mesh_vertices(vertices, &lane_point_values[0][0], LANE_POINT_COUNT);
            update_mesh_colors_pitch_class(pitch_class_colors, &chroma_index_field[0][0], &chroma_strength_field[0][0], LANE_POINT_COUNT);
            expand_mesh_colors_flat(flat_colors, pitch_class_colors, mesh_a.indices);

            //NOTE: by pure accident i was forgetting to update the normals here. This lead me to an intersting finding... position lights actually shade faces based on:
            // 1. Normals themselves (ofc -- in this case static, and the result of `GenMeshPlane` filling everything as {0.0f, 1.0f, 0.0f})
            // 2. BUT ALSO "direction from vertex -> position light" **This will actually CHANGE as the vertex positions animate with the terrain!!!!**
            //    - what results with static upfacing normals and then animated/dynamic vertex positions is a very strange but cool effect of like a thundercloud where
            //    - the lightings actual source is very hard to see but in a way that emulates faint almost curtain like shading...
            /*
            update_mesh_normals_smooth(normals, vertices, LANE_POINT_COUNT);
            */

            update_mesh_vertices_flat(flat_vertices, vertices, mesh_a.indices);
            update_mesh_normals_flat(flat_normals, flat_vertices);

            build_mesh_smooth(&mesh_a, vertices, normals, colors, MESH_VERTEX_COUNT);
            build_mesh_smooth(&mesh_b, vertices, normals, pitch_class_colors, MESH_VERTEX_COUNT);
            build_mesh_flat(&flat_mesh, flat_vertices, flat_normals, flat_colors);
        }
        update_camera_orbit(&camera, GetFrameTime());
        update_padmouse(GetFrameTime(), &camera);
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
        glLightfv(GL_LIGHT0, GL_POSITION, (const GLfloat[]){light0_position.x, light0_position.y, light0_position.z, 1.0f});
        DrawModelEx(model_a, MIDDLE, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);
        glDisable(GL_LIGHTING);
        draw_light_position_marker();

        glEnable(GL_LIGHTING);
        glShadeModel(GL_FLAT);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (const GLfloat[]){0.0f, 0.0f, 0.0f, 1.0f});
        glLightfv(GL_LIGHT0, GL_AMBIENT, (const GLfloat[]){0.0f, 0.0f, 0.0f, 1.0f});
        glLightfv(GL_LIGHT0, GL_DIFFUSE, (const GLfloat[]){1.0f, 1.0f, 1.0f, 1.0f});
        DrawModelEx(flat_model, TOP, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);
        glShadeModel(GL_SMOOTH);
        glDisable(GL_LIGHTING);

        // DrawModelEx(model_b, BOTTOM, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);

        model_a.meshes[0].colors = NULL;
        // model_a.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = saved_texture_id;
        // rlDisableDepthTest();
        // rlDisableBackfaceCulling();
        // DrawModelEx(model_a, MIDDLE, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);
        // rlEnableBackfaceCulling();
        // rlEnableDepthTest();
        // model_a.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = 0;

        DrawModelPointsEx(model_a, TOP, Y_AXIS, 0.0f, DEFAULT_SCALE, BLUE);
        DrawModelWiresEx(model_a, BOTTOM, Y_AXIS, 0.0f, DEFAULT_SCALE, MAGENTA);
        if (is_paused) {
            draw_paused_wave_cursor_lane_marker();
        }

        model_a.meshes[0].colors = saved_colors;

        EndMode3D();
        DrawTextEx(font, TextFormat("%2i FPS", GetFPS()), (Vector2){50.0f, 440.0f}, FONT_SIZE, 0.0f, WHITE);
        draw_playback_inspection_hud();
        draw_padmouse();
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
    RL_FREE(fft_data.spectrum_history_levels);
    RL_FREE(fft_data.prev_spectrum_bin_levels);
    RL_FREE(fft_data.work_buffer);
    UnloadFont(font);
    CloseWindow();
    return 0;
}

static void update_playback_controls_fft(void) {
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

                apply_blackman_window();
                shz_fft((shz_complex_t*)fft_data.work_buffer, (size_t)ANALYSIS_WINDOW_SIZE_IN_FRAMES);
                build_spectrum();

                float* spectrum_bin_levels =
                    fft_data.spectrum_history_levels[(fft_data.history_pos - 1 + ANALYSIS_FFT_HISTORY_FRAME_COUNT) % ANALYSIS_FFT_HISTORY_FRAME_COUNT];
                advance_lane_history(&lane_point_values[0][0], LANE_POINT_COUNT);
                advance_lane_history_u8(&chroma_index_field[0][0], LANE_POINT_COUNT);
                advance_lane_history(&chroma_strength_field[0][0], LANE_POINT_COUNT);
                for (int i = 0; i < LANE_POINT_COUNT; i++) {
                    lane_point_values[0][i] = spectrum_bin_levels[(i * (ANALYSIS_SPECTRUM_BIN_COUNT - 1)) / (LANE_POINT_COUNT - 1)];
                }
                build_pitch_class_color_field(&chroma_index_field[0][0],
                                              &chroma_strength_field[0][0],
                                              &lane_point_values[0][0],
                                              spectrum_bin_levels,
                                              LANE_POINT_COUNT,
                                              1,
                                              ANALYSIS_SPECTRUM_BIN_COUNT - 1);
                update_onset_interpolation_factor_fft(&fft_data);
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
        MEMSET(fft_data.prev_spectrum_bin_levels, 0, sizeof(float) * ANALYSIS_SPECTRUM_BIN_COUNT);
        MEMSET(fft_data.spectrum_history_levels, 0, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]) * ANALYSIS_FFT_HISTORY_FRAME_COUNT);
        MEMSET(lane_point_values, 0, sizeof(lane_point_values));
        MEMSET(chroma_index_field, 0, sizeof(chroma_index_field));
        MEMSET(chroma_strength_field, 0, sizeof(chroma_strength_field));
        fft_data.history_pos = 0;
        fft_data.frame_index = 0;
        onset_interpolation_factor = 0.0f;

        for (int i = ANALYSIS_FFT_HISTORY_FRAME_COUNT - 1; i >= 0; i--) {
            int replay_frame_offset = (i * AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES) % wave.frameCount;
            int chunk_start_frame = (wave_cursor + wave.frameCount - replay_frame_offset) % wave.frameCount;

            for (int j = 0; j < ANALYSIS_WINDOW_SIZE_IN_FRAMES; j++) {
                int src = (chunk_start_frame + j) % wave.frameCount;
                analysis_window_samples[j] = (float)wave_pcm16[src] / ANALYSIS_PCM16_UPPER_BOUND;
            }

            apply_blackman_window();
            shz_fft((shz_complex_t*)fft_data.work_buffer, (size_t)ANALYSIS_WINDOW_SIZE_IN_FRAMES);
            build_spectrum();

            float* spectrum_bin_levels =
                fft_data.spectrum_history_levels[(fft_data.history_pos - 1 + ANALYSIS_FFT_HISTORY_FRAME_COUNT) % ANALYSIS_FFT_HISTORY_FRAME_COUNT];
            advance_lane_history(&lane_point_values[0][0], LANE_POINT_COUNT);
            advance_lane_history_u8(&chroma_index_field[0][0], LANE_POINT_COUNT);
            advance_lane_history(&chroma_strength_field[0][0], LANE_POINT_COUNT);
            for (int i = 0; i < LANE_POINT_COUNT; i++) {
                lane_point_values[0][i] = spectrum_bin_levels[(i * (ANALYSIS_SPECTRUM_BIN_COUNT - 1)) / (LANE_POINT_COUNT - 1)];
            }
            build_pitch_class_color_field(&chroma_index_field[0][0],
                                          &chroma_strength_field[0][0],
                                          &lane_point_values[0][0],
                                          spectrum_bin_levels,
                                          LANE_POINT_COUNT,
                                          1,
                                          ANALYSIS_SPECTRUM_BIN_COUNT - 1);
            update_onset_interpolation_factor_fft(&fft_data);
        }

        analysis_dirty = 1;
    }

    if (analysis_dirty) {
        update_mesh_vertices(vertices, &lane_point_values[0][0], LANE_POINT_COUNT);
        update_mesh_colors_pitch_class(pitch_class_colors, &chroma_index_field[0][0], &chroma_strength_field[0][0], LANE_POINT_COUNT);
        expand_mesh_colors_flat(flat_colors, pitch_class_colors, mesh_a.indices);
        update_mesh_normals_smooth(normals, vertices, LANE_POINT_COUNT);
        update_mesh_vertices_flat(flat_vertices, vertices, mesh_a.indices);
        update_mesh_normals_flat(flat_normals, flat_vertices);
        build_mesh_smooth(&mesh_a, vertices, normals, colors, MESH_VERTEX_COUNT);
        build_mesh_smooth(&mesh_b, vertices, normals, pitch_class_colors, MESH_VERTEX_COUNT);
        build_mesh_flat(&flat_mesh, flat_vertices, flat_normals, flat_colors);
    }
}
