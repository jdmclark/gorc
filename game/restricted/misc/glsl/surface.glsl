#ifdef VERTEXPROGRAM

void main() {
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_FrontColor = gl_Color;
}

#else if FRAGMENTPROGRAM

uniform sampler2D diffuse;
uniform sampler2D light;
uniform vec4 sector_tint;

void main() {
	vec4 diffuse = texture2D(diffuse, gl_TexCoord[0].st) * gl_Color;
	vec4 emissive = texture2D(light, gl_TexCoord[0].st);
	emissive.a = length(emissive.rgb);

	vec4 diffuse_emissive = mix(diffuse, emissive, emissive.a);

	vec4 diffuse_emissive_tint = saturate(diffuse_emissive + sector_tint);
	gl_FragColor = diffuse_emissive_tint;
	gl_FragColor.a = diffuse.a;
}

#endif

