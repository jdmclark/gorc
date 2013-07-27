uniform vec3 ceiling_sky_offset;
uniform sampler2D diffuse;
uniform sampler2D light;
uniform vec4 sector_tint;

varying vec4 ceiling_sky_view_position;
varying vec4 ceiling_sky_normal;
varying vec4 ceiling_sky_point;
varying vec4 ceiling_sky_u;
varying vec4 ceiling_sky_v;

#ifdef VERTEXPROGRAM

void main() {
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	
	ceiling_sky_view_position = gl_ModelViewMatrix * gl_Vertex;
	ceiling_sky_normal = gl_ModelViewMatrix * vec4(0.0, 0.0, -1.0, 0.0);
	ceiling_sky_u = gl_ModelViewMatrix * vec4(1.0, 0.0, 0.0, 0.0);
	ceiling_sky_v = gl_ModelViewMatrix * vec4(0.0, 1.0, 0.0, 0.0);	
	ceiling_sky_point = gl_ModelViewMatrix * vec4(0.0, 0.0, ceiling_sky_offset.z, 1.0);
}

#else if FRAGMENTPROGRAM

void ceiling_sky_texture_coordinates(out vec2 tex_coords) {
	float sky_t = dot(ceiling_sky_point, ceiling_sky_normal) / dot(ceiling_sky_view_position, ceiling_sky_normal);
	vec3 out_intersection = (ceiling_sky_view_position * sky_t) - ceiling_sky_point;
	
	tex_coords = vec2(dot(out_intersection, ceiling_sky_u), dot(out_intersection, ceiling_sky_v));
}

void main() {	
	vec2 tex_coords;
	ceiling_sky_texture_coordinates(tex_coords);
	tex_coords += ceiling_sky_offset.xy / 8.0;
	tex_coords *= 16.0 / textureSize2D(diffuse, 0);
	
	vec4 diffuse = texture2D(diffuse, tex_coords);
	vec4 diffuse_color = diffuse;
	vec4 diffuse_color_tint = diffuse_color * sector_tint;
	vec4 diffuse_color_tint_mix = mix(diffuse_color, diffuse_color_tint, sector_tint.a);
	gl_FragColor = diffuse_color_tint_mix;
	gl_FragColor.a = diffuse_color.a;
	gl_FragDepth = gl_DepthRange.far - 0.000002;
}

#endif
