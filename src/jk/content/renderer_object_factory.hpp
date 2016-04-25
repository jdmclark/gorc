#pragma once

#include "math/grid.hpp"
#include "math/color.hpp"
#include "content/id.hpp"

namespace gorc {

    class renderer_object_factory {
    public:
        virtual ~renderer_object_factory();

        virtual void set_material_image(material_id,
                                        int cel,
                                        int channel,
                                        grid<color_rgba8> const &img) = 0;
    };

}
