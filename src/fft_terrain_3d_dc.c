#define FFFFTT_PROFILE_FFT_TERRAIN_3D
#include "fffftt.h"
#include <GL/gl.h>

#define LINE_WIDTH_RASTER_PIXELS 1.0f
#define POINT_SIZE_RASTER_PIXELS 3.0f
#define ONSET_LAG_FRAMES 1
#define ONSET_ENVELOPE_RADIUS 2

static const char* domain = "FFT-TERRAIN-3D-DC";

static Mesh mesh_a = {0};
static Mesh mesh_b = {0};
static Mesh flat_mesh = {0};

static Model model_a = {0};
static Model model_b = {0};
static Model flat_model = {0};

#define PITCH_CLASS_COUNT 12
//https://en.wikipedia.org/wiki/Equal_temperament
#define PITCH_CLASS_A4_FREQUENCY_HZ 440.0f
#define PITCH_CLASS_INVERSE_LN_2 1.4426950408889634f
#define PITCH_CLASS_SEMITONES_PER_OCTAVE 12.0f
#define PITCH_CLASS_TUNING_RADIUS_SEMITONES 0.85f
#define PITCH_CLASS_VISIBILITY_MIN 0.02f
#define PITCH_CLASS_VISIBILITY_MAX 0.75f
#define C9 8372.02f
#define Db9 8870.0f
#define D9 9398.0f
#define Eb9 9974.0f
#define E9 10548.0f
#define F9 11176.0f
#define Gb9 11840.0f
#define G9 12544.0f
#define Ab9 13290.0f
#define A9 14080.0f
#define Bb9 14918.0f
#define B9 15804.0f
#define REF_HZ_LUT {C9, Db9, D9, Eb9, E9, F9, Gb9, G9, Ab9, A9, Bb9, B9}
#define REF_HZ_LOOKUP(index) (((const float[PITCH_CLASS_COUNT])REF_HZ_LUT)[(index)])
#define ACCIDENTALS_LUT {1.0f, 0.5f, 1.0f, 0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.5f, 1.0f, 0.5f, 1.0f}
#define ACCIDENTAL_LOOKUP(index) (((const float[PITCH_CLASS_COUNT])ACCIDENTALS_LUT)[(index)])
#define WUXING_COLOR_LOOKUP(index) (((const Color[PITCH_CLASS_COUNT])WUXING_PITCH_CLASS_LUT)[(index)])

static void update_onset_interpolation_factor(FFTData* fft_data);
static void build_pitch_class_color_field(void);
static void update_mesh_colors_pitch_class(Color* colors);
static Color sample_pitch_class_palette(unsigned char chroma_index, float confidence);
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
    fft_data.tapback_pos = ANALYSIS_TAPBACK_POS_DEFAULT;
    fft_data.work_buffer = RL_CALLOC(ANALYSIS_WINDOW_SIZE_IN_FRAMES, sizeof(FFTComplex));
    fft_data.prev_spectrum_bin_levels = RL_CALLOC(ANALYSIS_SPECTRUM_BIN_COUNT, sizeof(float));
    fft_data.spectrum_history_levels = RL_CALLOC(ANALYSIS_FFT_HISTORY_FRAME_COUNT, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]));

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
    // wave = LoadWave(RD_DDS_FFM_22K_WAV);
    wave = LoadWave(RD_RAMA_22K_WAV);
    //wave = LoadWave(RD_SHADERTOY_EXPERIMENT_22K_WAV);
    WaveFormat(&wave, SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    audio_stream = LoadAudioStream(SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    PlayAudioStream(audio_stream);

    wave_pcm16 = (int16_t*)wave.data;

    Camera3D camera = {
        .position = (Vector3){1.45625f * 3.0f, 1.345f * 3.0f, -1.36625f * 3.0f},
        .target = (Vector3){0.0f, 0.25f, 0.0f},
        .up = Y_AXIS,
        .fovy = 8.0f,
        .projection = CAMERA_ORTHOGRAPHIC,
    };

    mesh_a = GenMeshPlane(1.0f, 1.0f, LANE_POINT_COUNT - 1, LANE_COUNT - 1);
    // NOTE: FOR USING CUSTOM LANE TOPOLOGY APPROACH WITH "WIRE DRAWS":
    // mesh_a.triangleCount = LINE_SEGMENT_COUNT; // TODO: defeats the purpose of genMesh...
    // mesh_a.indices = RL_CALLOC(LINE_INDEX_COUNT, sizeof(unsigned short)); // TODO: defeats the purpose of genMesh...
    // fill_mesh_indices_lane_topology(mesh_a.indices);

    update_mesh_vertices(mesh_a.vertices);
    mesh_a.colors = RL_CALLOC(mesh_a.vertexCount, sizeof(Color));

    Texture2D lane_mask_texture = build_lane_mask(mesh_a.texcoords);
    // Texture2D lane_mask_texture = build_lane_mask_glow(mesh_a.texcoords);

    model_a = LoadModelFromMesh(mesh_a);
    unsigned char* saved_colors = model_a.meshes[0].colors;
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

    update_mesh_vertices(vertices);
    update_mesh_normals_smooth(normals, vertices);
    update_mesh_colors_pitch_class(pitch_class_colors);

    update_mesh_vertices_flat(flat_vertices, vertices, mesh_a.indices);
    update_mesh_normals_flat(flat_normals, flat_vertices);

    build_mesh_smooth(&mesh_a, vertices, normals, colors);
    build_mesh_smooth(&mesh_b, vertices, normals, pitch_class_colors);
    build_mesh_flat(&flat_mesh, flat_vertices, flat_normals, flat_colors);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

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
            advance_lane_history(&lane_point_values[0][0]);
            advance_lane_history_u8(&chroma_index_field[0][0]);
            advance_lane_history(&chroma_strength_field[0][0]);
            for (int i = 0; i < LANE_POINT_COUNT; i++) {
                lane_point_values[0][i] = spectrum_bin_levels[(i * (ANALYSIS_SPECTRUM_BIN_COUNT - 1)) / (LANE_POINT_COUNT - 1)];
            }
            build_pitch_class_color_field();
            update_onset_interpolation_factor(&fft_data);
            audio_dirty = 1;
        }

        if (audio_dirty) {
            update_mesh_vertices(vertices);
            update_mesh_colors_pitch_class(pitch_class_colors);

            //NOTE: by pure accident i was forgetting to update the normals here. This lead me to an intersting finding... position lights actually shade faces based on:
            // 1. Normals themselves (ofc -- in this case static, and the result of `GenMeshPlane` filling everything as {0.0f, 1.0f, 0.0f})
            // 2. BUT ALSO "direction from vertex -> position light" **This will actually CHANGE as the vertex positions animate with the terrain!!!!**
            //    - what results with static upfacing normals and then animated/dynamic vertex positions is a very strange but cool effect of like a thundercloud where
            //    - the lightings actual source is very hard to see but in a way that emulates faint almost curtain like shading...
            /*
            update_mesh_normals_smooth(normals, vertices);
            */

            // update_mesh_vertices_flat(flat_vertices, vertices, mesh_a.indices);
            // update_mesh_normals_flat(flat_normals, flat_vertices);

            build_mesh_smooth(&mesh_a, vertices, normals, colors);
            build_mesh_smooth(&mesh_b, vertices, normals, pitch_class_colors);
            // build_mesh_flat(&flat_mesh, flat_vertices, flat_normals, flat_colors);
        }
        update_camera_orbit(&camera, GetFrameTime());
        update_light_constants();

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
        glLightfv(GL_LIGHT0, GL_POSITION, (const GLfloat[]){1.330f, 1.345f, -1.418f, 1.0f});
        // DrawModelEx(model_a, MIDDLE, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);
        glDisable(GL_LIGHTING);

        // glEnable(GL_LIGHTING);
        // glShadeModel(GL_FLAT);
        // glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (const GLfloat[]){0.0f, 0.0f, 0.0f, 1.0f});
        // glLightfv(GL_LIGHT0, GL_AMBIENT, (const GLfloat[]){0.0f, 0.0f, 0.0f, 1.0f});
        // glLightfv(GL_LIGHT0, GL_DIFFUSE, (const GLfloat[]){1.0f, 1.0f, 1.0f, 1.0f});
        // DrawModelEx(flat_model, TOP, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);
        // glShadeModel(GL_SMOOTH);
        // glDisable(GL_LIGHTING);

        DrawModelEx(model_b, MIDDLE, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);

        model_a.meshes[0].colors = NULL;
        // model_a.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = saved_texture_id;
        // rlDisableDepthTest();
        // rlDisableBackfaceCulling();
        // DrawModelEx(model_a, MIDDLE, Y_AXIS, 0.0f, DEFAULT_SCALE, WHITE);
        // rlEnableBackfaceCulling();
        // rlEnableDepthTest();
        // model_a.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id = 0;

        // DrawModelPointsEx(model_a, TOP, Y_AXIS, 0.0f, DEFAULT_SCALE, BLUE);
        // DrawModelWiresEx(model_a, BOTTOM, Y_AXIS, 0.0f, DEFAULT_SCALE, MAGENTA);

        model_a.meshes[0].colors = saved_colors;

        EndMode3D();
        DrawFPS(550, 440);
        EndDrawing();
    }

    UnloadTexture(lane_mask_texture);
    UnloadModel(model_a);
    UnloadModel(model_b);
    UnloadModel(flat_model);
    UnloadAudioStream(audio_stream);
    UnloadWave(wave);
    CloseAudioDevice();
    RL_FREE(fft_data.spectrum_history_levels);
    RL_FREE(fft_data.prev_spectrum_bin_levels);
    RL_FREE(fft_data.work_buffer);
    CloseWindow();
    return 0;
}

static void update_onset_interpolation_factor(FFTData* fft_data) {
    // librosa onset_strength convention:
    // https://librosa.org/doc/main/generated/librosa.onset.onset_strength.html
    // TODO: onset_detect would be a later peak-pick stage is more complicated...
    // https://librosa.org/doc/main/generated/librosa.onset.onset_detect.html
    if (fft_data->frame_index <= ONSET_LAG_FRAMES) {
        return;
    }

    int current_index = (fft_data->history_pos - 1 + ANALYSIS_FFT_HISTORY_FRAME_COUNT) % ANALYSIS_FFT_HISTORY_FRAME_COUNT;
    int lag_index = (fft_data->history_pos - 1 - ONSET_LAG_FRAMES + ANALYSIS_FFT_HISTORY_FRAME_COUNT) % ANALYSIS_FFT_HISTORY_FRAME_COUNT;
    float* spectrum = fft_data->spectrum_history_levels[current_index];
    float* lag_spectrum = fft_data->spectrum_history_levels[lag_index];
    float flux_sum = 0.0f;
    for (int i = 0; i < ANALYSIS_SPECTRUM_BIN_COUNT; i++) {
        float ref = lag_spectrum[i];
        for (int j = -ONSET_ENVELOPE_RADIUS; j <= ONSET_ENVELOPE_RADIUS; j++) {
            int k = i + j;
            if (k < 0 || k >= ANALYSIS_SPECTRUM_BIN_COUNT) {
                continue;
            }
            ref = FMAXF(ref, lag_spectrum[k]);
        }
        float flux = spectrum[i] - ref;
        if (flux > 0.0f) {
            flux_sum += flux;
        }
    }
    float onset_strength = flux_sum / (float)ANALYSIS_SPECTRUM_BIN_COUNT;
    float onset_strength_normalized = CLAMP((onset_strength - ONSET_STRENGTH_MIN) / (ONSET_STRENGTH_MAX - ONSET_STRENGTH_MIN), 0.0f, 1.0f);
    float onset_rate = ONSET_DECAY_RATE;
    if (onset_strength_normalized > onset_interpolation_factor) {
        onset_rate = ONSET_ATTACK_RATE;
    }
    onset_interpolation_factor = LERP(onset_interpolation_factor, onset_strength_normalized, onset_rate);
    onset_interpolation_factor = CLAMP(onset_interpolation_factor, 0.0f, 1.0f);
}

static void build_pitch_class_color_field(void) {
    // https://librosa.org/doc/main/generated/librosa.feature.chroma_stft.html
    for (int i = 0; i < LANE_POINT_COUNT; i++) {
        int center_bin = (i * (ANALYSIS_SPECTRUM_BIN_COUNT - 1)) / (LANE_POINT_COUNT - 1);
        int left_center_bin = center_bin;
        int right_center_bin = center_bin;
        if (i > 0) {
            left_center_bin = ((i - 1) * (ANALYSIS_SPECTRUM_BIN_COUNT - 1)) / (LANE_POINT_COUNT - 1);
        }
        if (i < LANE_POINT_COUNT - 1) {
            right_center_bin = ((i + 1) * (ANALYSIS_SPECTRUM_BIN_COUNT - 1)) / (LANE_POINT_COUNT - 1);
        }

        int bin_min = (i > 0) ? ((left_center_bin + center_bin) / 2) : 1;
        int bin_max = (i < LANE_POINT_COUNT - 1) ? ((center_bin + right_center_bin + 1) / 2) : (ANALYSIS_SPECTRUM_BIN_COUNT - 1);
        if (bin_max < bin_min) {
            bin_max = bin_min;
        }

        float chroma_energy[PITCH_CLASS_COUNT] = {0};
        float band_energy_sum = 0.0f;
        float band_peak = 0.0f;
        float locality_radius_bins = 0.5f * (float)(bin_max - bin_min + 1);
        if (locality_radius_bins < 1.0f) {
            locality_radius_bins = 1.0f;
        }

        for (int j = bin_min; j <= bin_max; j++) {
            float re = fft_data.work_buffer[j].real;
            float im = fft_data.work_buffer[j].imaginary;
            float bin_mag = SQRTF(re * re + im * im) / (float)ANALYSIS_WINDOW_SIZE_IN_FRAMES;
            if (bin_mag <= ANALYSIS_MIN_LOG_MAGNITUDE) {
                continue;
            }

            float bin_hz = ((float)j * (float)ANALYSIS_SAMPLE_RATE) / (float)ANALYSIS_WINDOW_SIZE_IN_FRAMES;
            if (bin_hz <= 0.0f) {
                continue;
            }

            float locality_gain = 1.0f - (FABSF((float)j - (float)center_bin) / locality_radius_bins);
            locality_gain = CLAMP(locality_gain, 0.0f, 1.0f);
            locality_gain = LERP(0.35f, 1.0f, locality_gain);

            band_energy_sum += bin_mag;
            band_peak = FMAXF(band_peak, bin_mag);

            for (int k = 0; k < PITCH_CLASS_COUNT; k++) {
                float ref_hz = REF_HZ_LOOKUP(k);
                float octave_offset = FLOORF((LOGF(ref_hz / bin_hz) * PITCH_CLASS_INVERSE_LN_2) + 0.5f);
                float folded_hz = ref_hz / POWF(2.0f, octave_offset);
                float semitone_error = FABSF(PITCH_CLASS_SEMITONES_PER_OCTAVE * LOGF(bin_hz / folded_hz) * PITCH_CLASS_INVERSE_LN_2);
                float tuning_gain = CLAMP(1.0f - (semitone_error / PITCH_CLASS_TUNING_RADIUS_SEMITONES), 0.0f, 1.0f);
                tuning_gain = tuning_gain * tuning_gain * (3.0f - 2.0f * tuning_gain);
                if (tuning_gain <= 0.0f) {
                    continue;
                }

                chroma_energy[k] += bin_mag * locality_gain * tuning_gain;
            }
        }

        int best_chroma = 0;
        float best_energy = 0.0f;
        float next_energy = 0.0f;
        for (int j = 0; j < PITCH_CLASS_COUNT; j++) {
            float energy = chroma_energy[j];
            if (energy > best_energy) {
                next_energy = best_energy;
                best_energy = energy;
                best_chroma = j;
            } else if (energy > next_energy) {
                next_energy = energy;
            }
        }

        float ridge_level = lane_point_values[0][i];
        float dominance = best_energy / FMAXF(band_energy_sum, 1e-6f);
        float separation = (best_energy - next_energy) / FMAXF(best_energy, 1e-6f);
        float peak_strength = CLAMP(band_peak * 18.0f, 0.0f, 1.0f);
        float chroma_strength = ridge_level * 0.55f + peak_strength * 0.20f + CLAMP(dominance, 0.0f, 1.0f) * 0.15f + CLAMP(separation, 0.0f, 1.0f) * 0.10f;
        chroma_strength = CLAMP(chroma_strength, 0.0f, 1.0f);
        chroma_strength = POWF(chroma_strength, 0.65f);

        if (best_energy <= 0.0f || ridge_level <= 0.0f) {
            chroma_strength = 0.0f;
        }

        chroma_index_field[0][i] = best_chroma;
        chroma_strength_field[0][i] = chroma_strength;
    }
}

static void update_mesh_colors_pitch_class(Color* colors) {
    for (int i = 0; i < LANE_COUNT; i++) {
        for (int j = 0; j < LANE_POINT_COUNT; j++) {
            int k = i * LANE_POINT_COUNT + j;
            colors[k] = sample_pitch_class_palette(chroma_index_field[i][j], chroma_strength_field[i][j]);
        }
    }
}

static Color sample_pitch_class_palette(unsigned char chroma_index, float confidence) {
    Color base_color = WUXING_COLOR_LOOKUP(chroma_index);
    float visibility = CLAMP((confidence - PITCH_CLASS_VISIBILITY_MIN) / (PITCH_CLASS_VISIBILITY_MAX - PITCH_CLASS_VISIBILITY_MIN), 0.0f, 1.0f);
    if (visibility <= 0.0f) {
        return BLACK;
    }

    visibility = POWF(visibility, 0.45f);
    float accidental_gain = 0.70f + 0.30f * ACCIDENTAL_LOOKUP(chroma_index);
    float brightness = (0.35f + 0.65f * visibility) * accidental_gain;
    brightness = CLAMP(brightness, 0.0f, 1.0f);

    return (Color){
        (unsigned char)((float)base_color.r * brightness),
        (unsigned char)((float)base_color.g * brightness),
        (unsigned char)((float)base_color.b * brightness),
        DRAW_COLOR_CHANNEL_MAX,
    };
}

static void update_playback_controls_fft(void) {
    int analysis_dirty = 0;
    int rebuild_from_cursor = 0;

    if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
        reset_sticky_nav();
        if (!is_paused) {
            is_paused = true;
            wave_cursor = (wave_cursor + wave.frameCount - AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES) % wave.frameCount;
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
                advance_lane_history(&lane_point_values[0][0]);
                advance_lane_history_u8(&chroma_index_field[0][0]);
                advance_lane_history(&chroma_strength_field[0][0]);
                for (int i = 0; i < LANE_POINT_COUNT; i++) {
                    lane_point_values[0][i] = spectrum_bin_levels[(i * (ANALYSIS_SPECTRUM_BIN_COUNT - 1)) / (LANE_POINT_COUNT - 1)];
                }
                build_pitch_class_color_field();
                update_onset_interpolation_factor(&fft_data);
                analysis_dirty = 1;
            }
        }
    }

    if (is_paused && sticky_nav(GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
        wave_cursor = (wave_cursor + wave.frameCount - AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES) % wave.frameCount;
        rebuild_from_cursor = 1;
    } else if (is_paused && sticky_nav(GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
        wave_cursor = (wave_cursor + AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES) % wave.frameCount;
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
            advance_lane_history(&lane_point_values[0][0]);
            advance_lane_history_u8(&chroma_index_field[0][0]);
            advance_lane_history(&chroma_strength_field[0][0]);
            for (int i = 0; i < LANE_POINT_COUNT; i++) {
                lane_point_values[0][i] = spectrum_bin_levels[(i * (ANALYSIS_SPECTRUM_BIN_COUNT - 1)) / (LANE_POINT_COUNT - 1)];
            }
            build_pitch_class_color_field();
            update_onset_interpolation_factor(&fft_data);
        }

        analysis_dirty = 1;
    }

    if (analysis_dirty) {
        update_mesh_vertices(vertices);
        update_mesh_colors_pitch_class(pitch_class_colors);
        update_mesh_normals_smooth(normals, vertices);
        update_mesh_vertices_flat(flat_vertices, vertices, mesh_a.indices);
        update_mesh_normals_flat(flat_normals, flat_vertices);
        build_mesh_smooth(&mesh_a, vertices, normals, colors);
        build_mesh_smooth(&mesh_b, vertices, normals, pitch_class_colors);
        build_mesh_flat(&flat_mesh, flat_vertices, flat_normals, flat_colors);
    }
}
