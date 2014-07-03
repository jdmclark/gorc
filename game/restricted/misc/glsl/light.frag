#version 130

uniform vec3 light_position;
uniform vec3 camera_position;

varying vec3 point_light_dir;
varying vec3 point_cam_dir;
varying vec3 point_normal;

uniform sampler2D diffuse;
uniform float inv_light_radius_squared;
uniform float light_intensity;

void main() {
    vec3 norm_point_norm = normalize(point_normal);
    vec3 norm_light_dir = normalize(point_light_dir);
    vec3 norm_cam_dir = normalize(point_cam_dir);

    float light_distance = length(point_light_dir);
    vec4 diffuse = texture2D(diffuse, gl_TexCoord[0].st);
    float attenuation = 1.0 - (light_distance * light_distance) / inv_light_radius_squared;
    attenuation *= light_intensity;

    float specularity = 0.25;
    float shininess = 10.0;
    float specular_reflection = attenuation * specularity *
        pow(max(0.0, dot(reflect(-norm_light_dir, norm_point_norm), norm_cam_dir)), shininess);

    vec4 light_color = vec4(1.0, 1.0, 1.0, 1.0) * attenuation;
    vec4 diffuse_color = diffuse * light_color + specular_reflection;
    gl_FragColor = diffuse_color;
    gl_FragColor.a *= gl_Color.a;
}
