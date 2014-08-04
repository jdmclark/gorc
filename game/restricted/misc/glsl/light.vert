#version 130

uniform vec3 light_position;
uniform vec3 camera_position;

varying vec3 point_light_dir;
varying vec3 point_cam_dir;
varying vec3 point_normal;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

void main() {
    vec4 world_pos = model_matrix * gl_Vertex;
    gl_Position = projection_matrix * view_matrix * model_matrix * gl_Vertex;
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_FrontColor = gl_Color;
    point_normal = (model_matrix * vec4(gl_Normal, 0.0)).xyz;
    point_light_dir = light_position - world_pos.xyz;
    point_cam_dir = camera_position - world_pos.xyz;
}
