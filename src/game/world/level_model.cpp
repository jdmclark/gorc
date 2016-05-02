#include "level_model.hpp"
#include "utility/flag_set.hpp"
#include "content/content_manager.hpp"
#include "game/constants.hpp"

gorc::game::world::level_model::level_model(event_bus& bus, gorc::content_manager& content, service_registry const &svc,
        asset_ref<gorc::content::assets::level> level, asset_ref<content::assets::inventory> inv)
    : level(level), header(level->header), adjoins(level->adjoins), sectors(level->sectors),
      ecs(bus), services(&svc), script_model(services), inventory_model(inv),
      value_mapping(content, script_model, level) {

    services.add<cog::default_value_mapping>(value_mapping);

    // Copy surfaces and set up object data.
    std::copy(level->surfaces.begin(), level->surfaces.end(), std::back_inserter(surfaces));
    for(auto& sector : sectors) {
        for(int i = sector.first_surface; i < sector.first_surface + sector.surface_count; ++i) {
            surfaces[i].object_data.sector = sector.number;
            surfaces[i].object_data.surface_id = i;
        }
    }

    return;
}

gorc::game::world::components::thing& gorc::game::world::level_model::get_thing(thing_id id) {
    for(auto& t : ecs.find_component<components::thing>(id)) {
        return *t.second;
    }

    LOG_ERROR(format("get_thing: thing %d does not exist") % static_cast<int>(id));
    abort();
}

gorc::cog::source_type gorc::game::world::level_model::get_thing_source_type(cog::value id)
{
    if(id.get_type() == cog::value_type::cog) {
        return cog::source_type::cog;
    }
    else if(id.get_type() != cog::value_type::thing) {
        return cog::source_type::system;
    }

    thing_id tid = id;
    auto const &thing = get_thing(tid);
    switch(thing.type) {
    default:
        return cog::source_type::free;

    case flags::thing_type::Camera:
        return cog::source_type::camera;

    case flags::thing_type::Actor:
        return cog::source_type::actor;

    case flags::thing_type::Weapon:
        return cog::source_type::weapon;

    case flags::thing_type::Debris:
        return cog::source_type::debris;

    case flags::thing_type::Item:
        return cog::source_type::item;

    case flags::thing_type::Explosion:
        return cog::source_type::explosion;

    case flags::thing_type::cog:
        return cog::source_type::cog;

    case flags::thing_type::ghost:
        return cog::source_type::ghost;

    case flags::thing_type::Corpse:
        return cog::source_type::corpse;

    case flags::thing_type::Player:
        return cog::source_type::player;

    case flags::thing_type::Particle:
        return cog::source_type::particle;
    }
}

void gorc::game::world::level_model::send_to_linked(cog::message_type msg,
                                                    cog::value sender,
                                                    cog::value source,
                                                    cog::value param0,
                                                    cog::value param1,
                                                    cog::value param2,
                                                    cog::value param3)
{
    script_model.send_to_linked(msg,
                                sender,
                                source,
                                get_thing_source_type(source),
                                param0,
                                param1,
                                param2,
                                param3);

    thing_id tid = sender;
    if(tid.is_valid()) {
        auto const &thing = get_thing(tid);
        maybe_if(thing.cog, [&](asset_ref<cog::script> class_cog) {
                script_model.send(script_model.create_global_instance(class_cog),
                                  msg,
                                  sender,
                                  /* sender id */ cog::value(),
                                  source,
                                  param0,
                                  param1,
                                  param2,
                                  param3,
                                  "class");
            });

        maybe_if(thing.capture_cog, [&](cog_id capture_cog) {
                script_model.send(capture_cog,
                                  msg,
                                  sender,
                                  /* sender id */ cog::value(),
                                  source,
                                  param0,
                                  param1,
                                  param2,
                                  param3,
                                  "capture");
            });
    }
}
