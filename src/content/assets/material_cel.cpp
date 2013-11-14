#include "material_cel.h"

gorc::content::assets::material_cel::material_cel(GLuint diffuse, GLuint light, const box<2, int>& m_size)
	: diffuse(diffuse, m_size), light(light, m_size) {
	return;
}
