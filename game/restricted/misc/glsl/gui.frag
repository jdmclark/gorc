#version 130

uniform sampler2D diffuse;

void main() {
    gl_FragColor = texture2D(diffuse, gl_TexCoord[0].st) * gl_Color;
    if(gl_FragColor.a < 0.5) {
        discard;
    }
    gl_FragColor.a = 1.0;
}
