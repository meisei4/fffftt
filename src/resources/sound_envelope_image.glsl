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

uniform float u_line_render_width;
uniform float u_amplitude_scale;
uniform float u_row_spacing;
uniform float u_isometric_zoom;
uniform vec2 u_grid_center;

float get_envelope_from_buffer(int history_row, int bin_index) {
    float texture_v = 1.0 - (float(history_row) + 0.5) / float(NUMBER_OF_HISTORY_ROWS);
    float texture_u = (float(bin_index) + 0.5) / float(NUMBER_OF_BINS);
    return texture(iChannel0, vec2(texture_u, texture_v)).r;
}

vec2 compute_grid_position(vec3 envelope_coordinate) {
    float bin_index = envelope_coordinate.x;
    float envelope_value = envelope_coordinate.y;
    float history_row = envelope_coordinate.z;

    float effective_row = history_row * u_row_spacing;
    float proj_x = bin_index - effective_row;
    float proj_y = (bin_index + effective_row) * 0.5 - envelope_value * u_amplitude_scale;

    return vec2(proj_x, proj_y);
}

vec2 project_centered_envelope(vec3 envelope_coordinate) {
    vec2 grid_position = compute_grid_position(envelope_coordinate);
    float x = 0.5 * iResolution.x + (grid_position.x - u_grid_center.x) * u_isometric_zoom;
    float y = 0.5 * iResolution.y + (grid_position.y - u_grid_center.y) * u_isometric_zoom;
    return vec2(x, y);
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

    float intensity = 1.0 - smoothstep(u_line_render_width, u_line_render_width * 2.0, closest_distance);
    frag_color = vec4(vec3(intensity), 1.0);
}

void main() {
    vec4 color;
    mainImage(color, gl_FragCoord.xy);
    finalColor = color;
}
