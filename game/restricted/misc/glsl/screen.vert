#version 130

void main() {
    gl_Position = gl_Vertex;
    gl_TexCoord[0] = gl_MultiTexCoord0;
}
