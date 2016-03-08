#version 130

uniform vec3 ceiling_sky_offset;

varying vec4 ceiling_sky_view_position;
varying vec4 ceiling_sky_normal;
varying vec4 ceiling_sky_point;
varying vec4 ceiling_sky_u;
varying vec4 ceiling_sky_v;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

void main() {
    gl_Position = projection_matrix * view_matrix * model_matrix * gl_Vertex;

    mat4 model_view_matrix = view_matrix * model_matrix;

    ceiling_sky_view_position = model_view_matrix * gl_Vertex;
    ceiling_sky_normal = model_view_matrix * vec4(0.0, 0.0, -1.0, 0.0);
    ceiling_sky_u = model_view_matrix * vec4(1.0, 0.0, 0.0, 0.0);
    ceiling_sky_v = model_view_matrix * vec4(0.0, 1.0, 0.0, 0.0);
    ceiling_sky_point = model_view_matrix * vec4(0.0, 0.0, ceiling_sky_offset.z, 1.0);
}
