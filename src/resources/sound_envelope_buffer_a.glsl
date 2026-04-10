#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec3 iResolution;
uniform float iTime;
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform float u_front_lane_smoothing;

out vec4 finalColor;

#define LANE_COUNT 5
#define LANE_POINT_COUNT 64
#define WAVEFORM_BUFFER_SIZE 512
#define WAVEFORM_SAMPLES_PER_LANE_POINT (WAVEFORM_BUFFER_SIZE / LANE_POINT_COUNT)

void mainImage(out vec4 frag_color, in vec2 frag_coord) {
    vec2 uv = frag_coord.xy / iResolution.xy;
    float lane_height = 1.0 / float(LANE_COUNT);

    if (uv.y < 1.0 - lane_height) {
        frag_color = texture(iChannel0, uv + vec2(0.0, lane_height));
    } else {
        int i = int(floor(uv.x * float(LANE_POINT_COUNT)));
        if (i < 0)
            i = 0;
        if (i >= LANE_POINT_COUNT)
            i = LANE_POINT_COUNT - 1;

        float amplitude_sum = 0.0;
        for (int j = 0; j < WAVEFORM_SAMPLES_PER_LANE_POINT; j++) {
            float sample_x = (float(i * WAVEFORM_SAMPLES_PER_LANE_POINT + j) + 0.5) / float(WAVEFORM_BUFFER_SIZE);
            amplitude_sum += abs(texture(iChannel1, vec2(sample_x, 1.0)).r);
        }

        float lane_point_sample = (amplitude_sum / float(WAVEFORM_SAMPLES_PER_LANE_POINT)) * u_front_lane_smoothing;
        frag_color = vec4(lane_point_sample, lane_point_sample, lane_point_sample, 1.0);
    }
}

void main() {
    vec4 color;
    mainImage(color, gl_FragCoord.xy);
    finalColor = color;
}
