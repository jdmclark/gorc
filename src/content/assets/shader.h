#pragma once

#include <GL/glew.h>
#include "content/asset.h"
#include "content/loaders/shader_loader.h"

namespace gorc {
namespace content {
namespace assets {

class shader : public asset {
public:
	using loader = loaders::shader_loader;

	GLuint program;
};

}
}
}
