#pragma once

#include <GL/glew.h>
#include "content/asset.h"
#include "content/loaders/shader_loader.h"

namespace Gorc {
namespace Content {
namespace Assets {

class Shader : public Asset {
public:
	using Loader = Loaders::ShaderLoader;

	GLuint program;
};

}
}
}
