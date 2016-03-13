#pragma once

#include "utility/service_registry.hpp"
#include "libold/cog/verbs/table.hpp"
#include "libold/cog/compiler.hpp"
#include "libold/content/master_colormap.hpp"
#include "libold/base/content/loader_registry.hpp"
#include <memory>

namespace gorc {
namespace game {

namespace world {
class level_presenter;
}

class level_state {
public:
    service_registry services;
    cog::verbs::verb_table verb_table;
    std::unique_ptr<gorc::game::world::level_presenter> current_level_presenter;
    cog::compiler compiler;
    content::master_colormap colormap;

    level_state(service_registry const &parent_services);
};

}
}
