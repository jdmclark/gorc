#pragma once

#include <GL/glew.h>
#include <memory>

namespace gorc {
namespace content {
namespace assets {

class material_cel {
public:
	GLuint diffuse;
	GLuint light;

	material_cel(GLuint diffuse, GLuint Light);
	material_cel(const material_cel&) = delete;
	material_cel(material_cel&&);
	~material_cel();
};

}
}
}
