#include "fffftt.h"

static const char* domain = "PICKING-OUT-NOTES-DC";

// TODO: below until `main` is a stylistic idea trying to just mirror the original shadertoy code: https://www.shadertoy.com/view/tcG3Rm
//  WIP as an arguably absurd/pedantic style until maybe decide otherwise

#define GRID_COLUMN_COUNT 12 // #L33 vec2(12,10)
#define GRID_ROW_COUNT 10    // #L33 vec2(12,10)
#define CELL_SUBDIVISION_COUNT_X 8
#define CELL_SUBDIVISION_COUNT_Y 8

#define TWO_PI 6.28318530718 // #L8 #define TWO_PI 6.28318530718

#define C9 8372.02 // #L10 #define C9 8372.02
#define Db9 8870.  // #L11 #define Db9 8870.
#define D9 9398.   // #L12 #define D9 9398.
#define Eb9 9974.  // #L13 #define Eb9 9974.
#define E9 10548.  // #L14 #define E9 10548.
#define F9 11176.  // #L15 #define F9 11176.
#define Gb9 11840. // #L16 #define Gb9 11840.
#define G9 12544.  // #L17 #define G9 12544.
#define Ab9 13290. // #L18 #define Ab9 13290.
#define A9 14080.  // #L19 #define A9 14080.
#define Bb9 14918. // #L20 #define Bb9 14918.
#define B9 15804.  // #L21 #define B9 15804.

// #L23 float notes[] = float[](C9, Db9, D9,Eb9,E9,F9,Gb9,G9,Ab9,A9,Bb9,B9)
static const float notes[GRID_COLUMN_COUNT] = {C9, Db9, D9, Eb9, E9, F9, Gb9, G9, Ab9, A9, Bb9, B9};
// #L25 float flats[] = float[](1., 0.5, 1., 0.5, 1.,1.,0.5, 1., 0.5, 1., 0.5, 1.)
static const float flats[GRID_COLUMN_COUNT] = {1., .5, 1., .5, 1., 1., .5, 1., .5, 1., .5, 1.};
// #L27 vec3 cols[] = vec3[](vec3(1,1,0), vec3(0.5,1,0.1),vec3(0,0.8,0), vec3(0,.8,0.6),vec3(0,0.8,1), vec3(0,0.2,1), vec3(0.35,0,0.75),vec3(0.5,0,1), vec3(0.8,0,0.7),vec3(0.8,0,0), vec3(1,0.1,0), vec3(1,0.5,0))
static const float cols[GRID_COLUMN_COUNT][3] = {{1., 1., 0.},
                                                 {0.5, 1., 0.1},
                                                 {0, 0.8, 0.},
                                                 {0, 0.8, 0.6},
                                                 {0, 0.8, 1.},
                                                 {0, 0.2, 1.},
                                                 {0.35, 0, 0.75},
                                                 {0.5, 0, 1.},
                                                 {0.8, 0, 0.7},
                                                 {0.8, 0, 0.},
                                                 {1., 0.1, 0.},
                                                 {1., 0.5, 0.}};

static void picking_out_the_notes(const FFTData* fft_data) {
    float u = fft_data->tapback_pos / ((float)ANALYSIS_WINDOW_SIZE_IN_FRAMES / (float)ANALYSIS_SAMPLE_RATE); // #L32 u / iResolution.xy
    int history_position =
        (fft_data->history_pos - 1 - (int)FLOORF(u) + ANALYSIS_FFT_HISTORY_FRAME_COUNT) % ANALYSIS_FFT_HISTORY_FRAME_COUNT; // #L34 TO = floor(to)

    float cell_w = (float)SCREEN_WIDTH / GRID_COLUMN_COUNT; // #L33 U * vec2(12,10)
    float cell_h = (float)SCREEN_HEIGHT / GRID_ROW_COUNT;   // #L33 U * vec2(12,10)

    float* texture = fft_data->spectrum_history_levels[history_position]; // #L40 texture(iChannel0, ...)
    for (int TOy = 0; TOy < GRID_ROW_COUNT; TOy++) {
        for (int TOx = 0; TOx < GRID_COLUMN_COUNT; TOx++) {            // #L34 TO = floor(to)
            float to_x = (float)TOx;                                   // #L33 to.x
            float to_y = (float)TOy;                                   // #L33 to.y
            float col[3] = {cols[TOx][0], cols[TOx][1], cols[TOx][2]}; // #L36 cols[int(TO.x)]
            float flt = flats[TOx];                                    // #L36 flats[int(TO.x)]
            float frequency = notes[TOx] / exp2(9. - (float)TOy);      // #L39 frequency = notes[int(TO.x)] / exp2(9.-TO.y)
            float nyquist = (float)ANALYSIS_SAMPLE_RATE * .5;          // #L40 11025.
            // #L40 f = texture(iChannel0, vec2(frequency/11025.,0)).r
            float f = texture[(int)FLOORF(CLAMP(frequency / nyquist, 0., 1.) * (ANALYSIS_SPECTRUM_BIN_COUNT - 1) + .5)];
            // #L41 fa = texture(iChannel0, vec2((frequency*1.13)/11025.,0)).r
            float fa = texture[(int)FLOORF(CLAMP(frequency * 1.13 / nyquist, 0., 1.) * (ANALYSIS_SPECTRUM_BIN_COUNT - 1) + .5)];
            // #L42 fb = texture(iChannel0, vec2((frequency*0.87)/11025.,0)).r
            float fb = texture[(int)FLOORF(CLAMP(frequency * 0.87 / nyquist, 0., 1.) * (ANALYSIS_SPECTRUM_BIN_COUNT - 1) + .5)];
            // #L43 fc = texture(iChannel0, vec2((frequency*1.2)/11025.,0)).r
            float fc = texture[(int)FLOORF(CLAMP(frequency * 1.2 / nyquist, 0., 1.) * (ANALYSIS_SPECTRUM_BIN_COUNT - 1) + .5)];
            // #L44 fd = texture(iChannel0, vec2((frequency*0.8)/11025.,0)).r
            float fd = texture[(int)FLOORF(CLAMP(frequency * 0.8 / nyquist, 0., 1.) * (ANALYSIS_SPECTRUM_BIN_COUNT - 1) + .5)];

            float av = (f + fa + fb + fc + fd) / 5.; // #L45 av = (f+fa+fb+fc+fd)/5.
            float ff = (f - av) * 8. * f;            // #L46 ff = (f-av)*8.*f
            f = (f + f + f + ff + ff) / 4.;          // #L47 f=(f+f+f+ff+ff)/4.

            for (int j = 0; j < 8; j++) {                                           // #L35 fract(to).y
                float subcell_y = to_y + (float)j / 8.;                             // #L33 to.y
                float to_y_end = to_y + (float)(j + 1) / 8.;                        // #L33 to.y
                int py_min = (int)FLOORF((float)SCREEN_HEIGHT - to_y_end * cell_h); // #L33 to.y * iResolution.y
                int py_max = (int)CEILF((float)SCREEN_HEIGHT - subcell_y * cell_h); // #L33 to.y * iResolution.y
                if (py_max - py_min < 1)
                    py_max = py_min + 1;                  // #L35 abs(fract(to) - .5)
                for (int i = 0; i < 8; i++) {             // #L35 fract(to).x
                    float local_x = ((float)i + .5) / 8.; // #L35 fract(to).x
                    float local_y = ((float)j + .5) / 8.; // #L35 fract(to).y
                    float dx = FABSF(local_x - .5);       // #L35 abs(fract(to) - .5)
                    float dy = FABSF(local_y - .5);       // #L35 abs(fract(to) - .5)
                    float dist = FMAXF(dx, dy);           // #L38 dist = max(D.x, D.y)

                    //TODO: LOOK INTO USING shz_smoothstepf AND FULL REVIEW OF SHADER STUFF WITH sh4zam maths ofc!!!
                    float t = CLAMP(((f * f * f * f) - dist * 2.) / .01, 0., 1.); // #L48 smoothstep
                    float bright = t * t * (3. - 2. * t);                         // #L48 t*t*(3.-2.*t)
                    if (bright > 0.) {                                            // #L50 O = bright *
                        float subcell_x = to_x + (float)i / 8.;                   // #L33 to.x
                        float to_x_end = to_x + (float)(i + 1) / 8.;              // #L33 to.x
                        int px_min = (int)FLOORF(subcell_x * cell_w);             // #L33 to.x * iResolution.x
                        int px_max = (int)CEILF(to_x_end * cell_w);               // #L33 to.x * iResolution.x
                        if (px_max - px_min < 1)
                            px_max = px_min + 1; // #L35 abs(fract(to) - .5)
                        DrawRectangle(px_min,
                                      py_min,
                                      px_max - px_min,
                                      py_max - py_min,
                                      (Color){
                                          // #L50 O = bright * vec4(col*flt, 1)
                                          (unsigned char)(col[0] * flt * bright * 255.), // #L50 col.x*flt*bright
                                          (unsigned char)(col[1] * flt * bright * 255.), // #L50 col.y*flt*bright
                                          (unsigned char)(col[2] * flt * bright * 255.), // #L50 col.z*flt*bright
                                          255                                            // #L50 1
                                      });
                    }
                }
            }
        }
    }
}

int main(void) {
    // SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, domain);
    font = LoadFont(RD_FONT);

    fft_data.tapback_pos = ANALYSIS_TAPBACK_POS_DEFAULT;
    fft_data.work_buffer = RL_CALLOC(ANALYSIS_WINDOW_SIZE_IN_FRAMES, sizeof(FFTComplex));
    fft_data.prev_spectrum_bin_levels = RL_CALLOC(ANALYSIS_SPECTRUM_BIN_COUNT, sizeof(float));
    fft_data.raw_spectrum_history_levels = RL_CALLOC(ANALYSIS_FFT_HISTORY_FRAME_COUNT, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]));
    fft_data.spectrum_history_levels = RL_CALLOC(ANALYSIS_FFT_HISTORY_FRAME_COUNT, sizeof(float[ANALYSIS_SPECTRUM_BIN_COUNT]));

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES);
    load_audio_tracks();
    set_audio_track(DEFAULT_AUDIO_TRACK_SHADERTOY_EXPERIMENT);
    audio_stream = LoadAudioStream(SRC_SAMPLE_RATE, SRC_BIT_DEPTH, SRC_CHANNELS);
    PlayAudioStream(audio_stream);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            break;
        }

        update_playback_controls_fft_spectrum();

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
        }

        apply_blackman_window();
        shz_fft((shz_complex_t*)fft_data.work_buffer, (size_t)ANALYSIS_WINDOW_SIZE_IN_FRAMES);
        build_spectrum();

        update_audio_track_cycle();
        BeginDrawing();
        ClearBackground(BLACK);
        picking_out_the_notes(&fft_data);
        draw_playback_inspection_hud();
        DrawTextEx(font, TextFormat("%2i FPS", GetFPS()), (Vector2){50.0f, 440.0f}, FONT_SIZE, 0.0f, WHITE);
        EndDrawing();
    }

    UnloadAudioStream(audio_stream);
    unload_audio_tracks();
    CloseAudioDevice();
    RL_FREE(fft_data.raw_spectrum_history_levels);
    RL_FREE(fft_data.spectrum_history_levels);
    RL_FREE(fft_data.prev_spectrum_bin_levels);
    RL_FREE(fft_data.work_buffer);
    UnloadFont(font);
    CloseWindow();
    return 0;
}
