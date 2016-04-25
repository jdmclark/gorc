#pragma once

#include "jk/content/renderer_object_factory.hpp"
#include <GL/glew.h>
#include <tuple>
#include <map>

namespace gorc {

    class client_renderer_object_factory : public renderer_object_factory {
    private:
        std::map<std::tuple<material_id, int, int>, GLuint> material_images;

    public:
        ~client_renderer_object_factory();

        virtual void set_material_image(material_id,
                                        int cel,
                                        int channel,
                                        grid<color_rgba8> const &img) override;

        virtual GLuint get_material_image(material_id, int cel, int channel);
    };

}
