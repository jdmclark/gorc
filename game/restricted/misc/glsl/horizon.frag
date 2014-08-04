#version 130

uniform sampler2D diffuse;
uniform vec4 sector_tint;
uniform vec2 screen_size;
uniform vec3 horizon_sky_offset;

void horizon_sky_texture_coordinates(in vec4 vScreenPos, out vec2 tex_coords) {
    vec2 vTexCoord = vScreenPos.xy * horizon_sky_offset.z / screen_size.x;

    vTexCoord.x += horizon_sky_offset.x;
    vTexCoord.y += horizon_sky_offset.y;

    tex_coords = vTexCoord;
}

void main() {
    vec2 tex_coords;
    horizon_sky_texture_coordinates(gl_FragCoord, tex_coords);

    vec4 diffuse = texture2D(diffuse, tex_coords);
    vec4 diffuse_color = diffuse;
    vec4 diffuse_color_tint = diffuse_color * sector_tint;
    vec4 diffuse_color_tint_mix = mix(diffuse_color, diffuse_color_tint, sector_tint.a);
    gl_FragColor = diffuse_color_tint_mix;
    gl_FragColor.a = diffuse_color.a;
    gl_FragDepth = gl_DepthRange.far - 0.000002;
}
