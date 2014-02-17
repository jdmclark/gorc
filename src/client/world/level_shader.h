#pragma once

#include <GL/glew.h>
#include "framework/math/matrix.h"
#include "framework/math/box.h"
#include "framework/content/assets/shader.h"

namespace gorc {
namespace client {
namespace world {

class level_shader {
protected:
    const GLuint program;
    const GLint model_mat_ul, view_mat_ul, proj_mat_ul;

    level_shader(GLuint program, GLint model_mat_ul, GLint view_mat_ul, GLint proj_mat_ul);

public:
    virtual ~level_shader();

    void set_view_matrix(const matrix<4>& view_matrix);
    void set_projection_matrix(const matrix<4>& proj_matrix);
    void set_model_matrix(const matrix<4>& model_matrix);
};

class surface_shader : public level_shader {
    const GLint sector_tint_ul, diffuse_ul, light_ul;

public:
    surface_shader(const content::assets::shader& shader);

    void activate(const vector<3>& sector_tint);
};

class horizon_shader : public level_shader {
    const GLint sector_tint_ul, diffuse_ul, ssz_ul, offset_ul;
public:
    horizon_shader(const content::assets::shader& shader);

    void activate(const vector<3>& sector_tint, const vector<2>& offset, float horizon_pixels_per_rev,
            float horizon_distance, const box<2, int>& screen_size, const vector<3>& camera_look);
};

class ceiling_shader : public level_shader {
    const GLint diffuse_ul, sector_tint_ul, offset_ul;
public:
    ceiling_shader(const content::assets::shader& shader);

    void activate(const vector<3>& sector_tint, const vector<2>& offset, float ceiling_sky_z);
};

class light_shader : public level_shader {
    const GLint diffuse_ul, lightpos_ul, lightrad_ul, lightint_ul, campos_ul;
public:
    light_shader(const content::assets::shader& shader);

    void activate(const vector<3>& light_position, const vector<3>& camera_position, float light_radius, float light_intensity);
};

}
}
}
