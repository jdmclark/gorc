uniform vec3 light_position;
uniform vec3 camera_position;

varying vec3 point_light_dir;
varying vec3 point_cam_dir;
varying vec3 point_normal;

#ifdef VERTEXPROGRAM

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

#else if FRAGMENTPROGRAM

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
	float specular_reflection = vec4(1.0, 1.0, 1.0, 1.0) * attenuation * specularity *
		pow(max(0.0, dot(reflect(-norm_light_dir, norm_point_norm), norm_cam_dir)), shininess);
	
	vec4 light_color = vec4(1.0, 1.0, 1.0, 1.0) * attenuation;
	vec4 diffuse_color = diffuse * light_color + specular_reflection;
	gl_FragColor = diffuse_color;
	gl_FragColor.a *= gl_Color.a;
}

#endif
