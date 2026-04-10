#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec3 iResolution;
uniform float iTime;
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform int uAdvanceHistory;
uniform float uFrontLaneSmoothing;

out vec4 finalColor;

#define NUMBER_OF_BINS 64
#define NUMBER_OF_HISTORY_ROWS 5
#define WAVEFORM_SAMPLE_COUNT 512

const int samples_per_bin = WAVEFORM_SAMPLE_COUNT / NUMBER_OF_BINS;

float sample_waveform_envelope(int bin_index) {
    float sum_amplitude = 0.0;
    for (int i = 0; i < samples_per_bin; i++) {
        float sample_x = (float(bin_index * samples_per_bin + i) + 0.5) / float(WAVEFORM_SAMPLE_COUNT);
        float sample_value = texture(iChannel1, vec2(sample_x, 1.0)).r;
        sum_amplitude += abs(sample_value);
    }
    return sum_amplitude / float(samples_per_bin);
}

vec4 shift_envelope_history(vec2 uv_coordinates, float row_height) {
    return texture(iChannel0, uv_coordinates + vec2(0.0, row_height));
}

void mainImage(out vec4 frag_color, in vec2 frag_coord) {
    vec2 uv = frag_coord.xy / iResolution.xy;
    float row_height = 1.0 / float(NUMBER_OF_HISTORY_ROWS);

    if (uAdvanceHistory == 0) {
        frag_color = texture(iChannel0, uv);
        return;
    }

    if (uv.y < 1.0 - row_height) {
        frag_color = shift_envelope_history(uv, row_height);
    } else {
        int bin_index = int(floor(uv.x * float(NUMBER_OF_BINS)));
        if (bin_index < 0)
            bin_index = 0;
        if (bin_index >= NUMBER_OF_BINS)
            bin_index = NUMBER_OF_BINS - 1;

        float new_envelope = sample_waveform_envelope(bin_index) * uFrontLaneSmoothing;
        frag_color = vec4(new_envelope, new_envelope, new_envelope, 1.0);
    }
}

void main() {
    vec4 color;
    mainImage(color, gl_FragCoord.xy);
    finalColor = color;
}
