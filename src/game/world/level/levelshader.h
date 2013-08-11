#pragma once

#include <GL/glew.h>
#include "framework/math/matrix.h"
#include "framework/math/box.h"
#include "content/assets/shader.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class LevelShader {
protected:
	const GLuint program;
	const GLint model_mat_ul, view_mat_ul, proj_mat_ul;

	LevelShader(GLuint program, GLint model_mat_ul, GLint view_mat_ul, GLint proj_mat_ul);

public:
	virtual ~LevelShader();

	void SetViewMatrix(const Math::Matrix<float>& view_matrix);
	void SetProjectionMatrix(const Math::Matrix<float>& proj_matrix);
	void SetModelMatrix(const Math::Matrix<float>& model_matrix);
};

class SurfaceShader : public LevelShader {
	const GLint sector_tint_ul, diffuse_ul, light_ul;

public:
	SurfaceShader(const Content::Assets::Shader& shader);

	void Activate(const Math::Vector<3>& sector_tint);
};

class HorizonShader : public LevelShader {
	const GLint sector_tint_ul, diffuse_ul, ssz_ul, offset_ul;
public:
	HorizonShader(const Content::Assets::Shader& shader);

	void Activate(const Math::Vector<3>& sector_tint, const Math::Vector<2>& offset, float horizon_pixels_per_rev,
			float horizon_distance, const Math::Box<2, unsigned int>& screen_size, const Math::Vector<3>& camera_look);
};

class CeilingShader : public LevelShader {
	const GLint diffuse_ul, sector_tint_ul, offset_ul;
public:
	CeilingShader(const Content::Assets::Shader& shader);

	void Activate(const Math::Vector<3>& sector_tint, const Math::Vector<2>& offset, float ceiling_sky_z);
};

class LightShader : public LevelShader {
	const GLint diffuse_ul, lightpos_ul, lightrad_ul, lightint_ul, campos_ul;
public:
	LightShader(const Content::Assets::Shader& shader);

	void Activate(const Math::Vector<3>& light_position, const Math::Vector<3>& camera_position, float light_radius, float light_intensity);
};

}
}
}
}
