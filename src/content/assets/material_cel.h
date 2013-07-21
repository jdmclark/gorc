#pragma once

#include <GL/glew.h>
#include <memory>

namespace Gorc {
namespace Content {
namespace Assets {

class MaterialCel {
public:
	GLuint Diffuse;
	GLuint Light;

	MaterialCel(GLuint Diffuse, GLuint Light);
	MaterialCel(const MaterialCel&) = delete;
	MaterialCel(MaterialCel&&);
	~MaterialCel();
};

}
}
}
