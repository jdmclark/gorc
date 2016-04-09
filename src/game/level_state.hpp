#pragma once

#include "utility/service_registry.hpp"
#include "jk/cog/compiler/compiler.hpp"
#include "jk/cog/script/constant_table.hpp"
#include "jk/cog/script/verb_table.hpp"
#include "libold/content/master_colormap.hpp"
#include "content/loader_registry.hpp"
#include <memory>

namespace gorc {
namespace game {

namespace world {
class level_presenter;
}

class level_state {
public:
    service_registry services;
    cog::constant_table constants;
    cog::verb_table verbs;
    std::unique_ptr<gorc::game::world::level_presenter> current_level_presenter;
    cog::compiler compiler;
    content::master_colormap colormap;

    level_state(service_registry const &parent_services);
};

}
}
