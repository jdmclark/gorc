#include "level_state.hpp"
#include "game/world/level_presenter.hpp"
#include "cog/vm/default_verbs.hpp"

gorc::game::level_state::level_state(service_registry const &parent_services)
    : services(parent_services)
    , compiler(verbs, constants) {
    cog::default_populate_constant_table(constants);
    cog::default_populate_verb_table(verbs);

    services.add<gorc::cog::compiler>(compiler);
    services.add(colormap);
    return;
}
