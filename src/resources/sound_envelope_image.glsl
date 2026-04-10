#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec3 iResolution;
uniform sampler2D iChannel0;

out vec4 finalColor;

#define LANE_POINT_COUNT 64
#define LANE_COUNT 5
#define MAX_DISTANCE 1e6

uniform float u_amplitude_scale;
uniform float u_lane_spacing;
uniform float u_isometric_zoom;
uniform vec2 u_grid_center;
uniform float u_line_width_distance_pixels;

float get_lane_point_sample(int lane_index, int lane_point_index) {
    float texture_v = 1.0 - (float(lane_index) + 0.5) / float(LANE_COUNT);
    float texture_u = (float(lane_point_index) + 0.5) / float(LANE_POINT_COUNT);
    return texture(iChannel0, vec2(texture_u, texture_v)).r;
}

float distance_to_line(vec2 p, vec2 a, vec2 b) {
    vec2 ab = b - a;
    float denom = dot(ab, ab);
    float t = 0.0;
    if (denom > 0.0)
        t = clamp(dot(p - a, ab) / denom, 0.0, 1.0);
    vec2 q = a + t * ab;
    return distance(p, q);
}

void mainImage(out vec4 frag_color, in vec2 frag_coord) {
    float closest_distance = MAX_DISTANCE;

    for (int i = 0; i < LANE_COUNT; i++) {
        float lane_offset = float(i) * u_lane_spacing;

        for (int j = 0; j < LANE_POINT_COUNT - 1; j++) {
            float lane_point_sample = get_lane_point_sample(i, j);
            float grid_x = float(j) - lane_offset;
            float grid_y = 0.5 * (float(j) + lane_offset) - lane_point_sample * u_amplitude_scale;
            float x = 0.5 * iResolution.x + (grid_x - u_grid_center.x) * u_isometric_zoom;
            float y = 0.5 * iResolution.y + (grid_y - u_grid_center.y) * u_isometric_zoom;
            vec2 screen_position_left = vec2(x, y);

            lane_point_sample = get_lane_point_sample(i, j + 1);
            grid_x = float(j + 1) - lane_offset;
            grid_y = 0.5 * (float(j + 1) + lane_offset) - lane_point_sample * u_amplitude_scale;
            x = 0.5 * iResolution.x + (grid_x - u_grid_center.x) * u_isometric_zoom;
            // TODO: keeop this as a reference cause its gnarly bug
            // y = 0.5 * iResolution.y + (grid_y - u_grid_center.y) * u_isometric_zoom;
            y = 0.5 * iResolution.y + (grid_y - u_grid_center.y) * u_isometric_zoom;
            vec2 screen_position_right = vec2(x, y);

            float distance_to_wave_line = distance_to_line(frag_coord.xy, screen_position_left, screen_position_right);
            closest_distance = min(closest_distance, distance_to_wave_line);
        }
    }

    float intensity = 1.0 - smoothstep(u_line_width_distance_pixels, u_line_width_distance_pixels * 2.0, closest_distance);
    frag_color = vec4(vec3(intensity), 1.0);
}

void main() {
    vec4 color;
    mainImage(color, gl_FragCoord.xy);
    finalColor = color;
}
