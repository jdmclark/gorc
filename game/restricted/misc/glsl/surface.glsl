#ifdef VERTEXPROGRAM

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

void main() {
	gl_Position = projection_matrix * view_matrix * model_matrix * gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_FrontColor = gl_Color;
}

#else if FRAGMENTPROGRAM

uniform sampler2D diffuse;
uniform sampler2D light;
uniform vec4 sector_tint;

void main() {
	vec4 diffuse = texture2D(diffuse, gl_TexCoord[0].st);
	vec4 emissive = vec4(texture2D(light, gl_TexCoord[0].st).rgb, 0.0);
	vec4 diffuse_color = diffuse * gl_Color;
	vec4 diffuse_color_tint = diffuse_color * sector_tint;
	vec4 diffuse_color_tint_mix = mix(diffuse_color, diffuse_color_tint, sector_tint.a);
	gl_FragColor = emissive + diffuse_color_tint_mix;
	gl_FragColor.a = diffuse_color.a;
}

#endif

