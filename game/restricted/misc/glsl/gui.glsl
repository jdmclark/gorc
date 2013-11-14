#ifdef VERTEXPROGRAM

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

void main() {
	gl_Position = projection_matrix * view_matrix * model_matrix * gl_Vertex;
	gl_FrontColor = gl_Color;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}

#endif

#ifdef FRAGMENTPROGRAM

uniform sampler2D diffuse;

void main() {
	gl_FragColor = texture2D(diffuse, gl_TexCoord[0].st) * gl_Color;
	if(gl_FragColor.a < 0.5) {
		discard;
	}
}

#endif
