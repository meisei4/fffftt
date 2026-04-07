//DIRECT COPY OF https://www.shadertoy.com/view/tcSXRz
#define total_waveform_buffer_size_in_samples 512.0

#define WHITE vec4(1.0, 1.0, 1.0, 1.0)
#define BLACK vec4(0.0, 0.0, 0.0, 1.0)
#define WAVEFORM_ROW 1.0
#define LINE_WIDTH 1.0

void mainImage(out vec4 frag_color, in vec2 frag_coord) {
    vec2 uv = frag_coord.xy / iResolution.xy;
    float cell_width = iResolution.x / total_waveform_buffer_size_in_samples;
    float sample_index = floor(frag_coord.x / cell_width);
    float sample_x = (sample_index + 0.5) / total_waveform_buffer_size_in_samples;
    ivec2 sample_coord = ivec2(int(sample_index), int(WAVEFORM_ROW));
    float sample_value = texelFetch(iChannel0, sample_coord, 0).r;
    float line_y = floor(sample_value * (iResolution.y - 1.0) + 0.5);
    float pixel_y = floor(frag_coord.y);
    float line = 1.0 - step(0.5 * LINE_WIDTH, abs(pixel_y - line_y));
    vec4 color = mix(BLACK, WHITE, line);
    frag_color = color;
}