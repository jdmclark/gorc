#include "level_state.hpp"
#include "game/world/level_presenter.hpp"

gorc::game::level_state::level_state(service_registry const &parent_services)
    : services(parent_services)
    , compiler(verb_table) {
    services.add(loaders);
    services.add<gorc::cog::compiler>(compiler);
    services.add(colormap);
    return;
}
