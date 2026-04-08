#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec3 iResolution;
uniform sampler2D iChannel0;

out vec4 finalColor;

#define NUMBER_OF_BINS 64
#define NUMBER_OF_HISTORY_ROWS 5

#define LINE_RENDER_MARGIN 0.23
#define MAX_DISTANCE 1e6

#define WHITE vec4(1.0, 1.0, 1.0, 1.0)
#define BLACK vec4(0.0, 0.0, 0.0, 1.0)

const float line_render_width = 0.75;
const float amplitude_scale = 120.0;
const float row_spacing = 9.0;
const float isometric_zoom = 3.0;

float get_envelope_from_buffer(int history_row, int bin_index) {
    float texture_v = 1.0 - (float(history_row) + 0.5) / float(NUMBER_OF_HISTORY_ROWS);
    float texture_u = (float(bin_index) + 0.5) / float(NUMBER_OF_BINS);
    return texture(iChannel0, vec2(texture_u, texture_v)).r;
}

vec2 project_envelope(vec3 envelope_coordinate) {
    float bin_index = envelope_coordinate.x;
    float envelope_value = envelope_coordinate.y;
    float history_row = envelope_coordinate.z;

    float effective_row = history_row * row_spacing;
    float proj_x = bin_index - effective_row;
    float proj_y = (bin_index + effective_row) * 0.5 - envelope_value * amplitude_scale;

    return vec2(proj_x, proj_y) * isometric_zoom;
}

vec2 compute_envelope_grid_center() {
    float max_bin = float(NUMBER_OF_BINS - 1);
    float max_row = float(NUMBER_OF_HISTORY_ROWS - 1);

    vec2 p0 = project_envelope(vec3(0.0, 0.0, 0.0));
    vec2 p1 = project_envelope(vec3(max_bin, 0.0, 0.0));
    vec2 p2 = project_envelope(vec3(0.0, 1.0, max_row));
    vec2 p3 = project_envelope(vec3(max_bin, 1.0, max_row));

    vec2 min_corner = min(min(p0, p1), min(p2, p3));
    vec2 max_corner = max(max(p0, p1), max(p2, p3));

    return (min_corner + max_corner) * 0.5;
}

vec2 project_centered_envelope(vec3 envelope_coordinate) {
    vec2 raw_projected_coordinate = project_envelope(envelope_coordinate);
    vec2 grid_center = compute_envelope_grid_center();
    vec2 screen_center = iResolution.xy * 0.5;
    return raw_projected_coordinate + (screen_center - grid_center);
}

float distance_to_line(vec2 p, vec2 a, vec2 b) {
    vec2 ab = b - a;
    float denom = dot(ab, ab);
    float t = 0.0;
    if (denom > 0.0) t = clamp(dot(p - a, ab) / denom, 0.0, 1.0);
    vec2 q = a + t * ab;
    return distance(p, q);
}

void mainImage(out vec4 frag_color, in vec2 frag_coord) {
    float closest_distance = MAX_DISTANCE;

    for (int history_row = 0; history_row < NUMBER_OF_HISTORY_ROWS; history_row++) {
        for (int bin_index = 0; bin_index < NUMBER_OF_BINS - 1; bin_index++) {
            float envelope_left = get_envelope_from_buffer(history_row, bin_index);
            float envelope_right = get_envelope_from_buffer(history_row, bin_index + 1);

            vec3 envelope_coordinate_left = vec3(float(bin_index), envelope_left, float(history_row));
            vec3 envelope_coordinate_right = vec3(float(bin_index + 1), envelope_right, float(history_row));

            vec2 screen_position_left = project_centered_envelope(envelope_coordinate_left);
            vec2 screen_position_right = project_centered_envelope(envelope_coordinate_right);

            float distance_to_wave_line = distance_to_line(frag_coord.xy, screen_position_left, screen_position_right);
            closest_distance = min(closest_distance, distance_to_wave_line);
        }
    }

    float intensity = 1.0 - smoothstep(line_render_width, line_render_width * 2.0, closest_distance);
    frag_color = vec4(vec3(intensity), 1.0);
}

void main() {
    vec4 color;
    mainImage(color, gl_FragCoord.xy);
    finalColor = color;
}