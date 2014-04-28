#include "level_model.h"
#include "base/utility/flag_set.h"
#include "base/content/content_manager.h"
#include "game/constants.h"

gorc::game::world::level_model::level_model(event_bus& parent_event_bus, gorc::content::content_manager&, cog::compiler&,
        const gorc::content::assets::level& level, const content::assets::inventory& inv)
    : level(level), header(level.header), adjoins(level.adjoins), sectors(level.sectors),
      surface_ecs(&parent_event_bus), effect_ecs(&parent_event_bus), inventory_model(inv) {

    // Copy surfaces and set up object data.
    std::copy(level.surfaces.begin(), level.surfaces.end(), std::back_inserter(surfaces));
    for(auto& sector : sectors) {
        for(int i = sector.first_surface; i < sector.first_surface + sector.surface_count; ++i) {
            surfaces[i].object_data.sector_id = sector.number;
            surfaces[i].object_data.surface_id = i;
        }
    }

    return;
}
