#include "material_cel.h"

Gorc::Content::Assets::MaterialCel::MaterialCel(GLuint Diffuse, GLuint Light)
	: Diffuse(Diffuse), Light(Light) {
	return;
}

Gorc::Content::Assets::MaterialCel::MaterialCel(MaterialCel&& mc) {
	Diffuse = mc.Diffuse;
	mc.Diffuse = 0;

	Light = mc.Light;
	mc.Light = 0;
}

Gorc::Content::Assets::MaterialCel::~MaterialCel() {
	if(Diffuse) {
		glDeleteTextures(1, &Diffuse);
	}

	if(Light) {
		glDeleteTextures(1, &Light);
	}
}
