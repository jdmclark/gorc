#include "material_cel.h"

gorc::content::assets::material_cel::material_cel(GLuint diffuse, GLuint Light)
	: diffuse(diffuse), light(Light) {
	return;
}

gorc::content::assets::material_cel::material_cel(material_cel&& mc) {
	diffuse = mc.diffuse;
	mc.diffuse = 0;

	light = mc.light;
	mc.light = 0;
}

gorc::content::assets::material_cel::~material_cel() {
	if(diffuse) {
		glDeleteTextures(1, &diffuse);
	}

	if(light) {
		glDeleteTextures(1, &light);
	}
}
