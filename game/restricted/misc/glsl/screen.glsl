#ifdef VERTEXPROGRAM

void main() {
	gl_Position = gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}

#endif

#ifdef FRAGMENTPROGRAM

uniform sampler2D diffuse;

void main() {
	gl_FragColor = texture2D(diffuse, gl_TexCoord[0].st);
}

#endif
