#include "weapon_controller_aspect.hpp"
#include "game/world/level_model.hpp"
#include "libold/content/flags/weapon_flag.hpp"
#include "game/world/sounds/sound_presenter.hpp"
#include "game/world/events/touched_thing.hpp"
#include "game/world/events/touched_surface.hpp"
#include "game/world/events/thing_created.hpp"

using gorc::game::world::aspects::weapon_controller_aspect;

void weapon_controller_aspect::touched_thing(thing_id toucher, thing_id touched_thing_id) {
    auto& projectile = presenter.model->get_thing(toucher);
    if(projectile.type != flags::thing_type::Weapon) {
        return;
    }

    flag_set<flags::weapon_flag> proj_flags(projectile.type_flags);

    auto& touched_thing = presenter.model->get_thing(touched_thing_id);

    if(projectile.parent_thing.has_value() && projectile.parent_thing == touched_thing.parent_thing) {
        // Projectiles fired by same player. Ignore collision.
        return;
    }

    // Skip projectiles moving away from thing they were fired by.
    if(projectile.parent_thing == touched_thing_id && (proj_flags & flags::weapon_flag::does_not_affect_parent)) {
        return;
    }

    if(touched_thing.type == flags::thing_type::Actor || touched_thing.type == flags::thing_type::Player) {
        if(proj_flags & flags::weapon_flag::explodes_on_thing) {
            maybe_if(projectile.explode, [&](thing_template_id explode) {
                    presenter.create_thing_at_thing(explode, toucher);
                });
        }
        else {
            maybe_if(projectile.flesh_hit, [&](thing_template_id flesh_hit) {
                    presenter.create_thing_at_thing(flesh_hit, toucher);
                });
        }

        presenter.damage_thing(touched_thing_id, projectile.damage, flag_set<flags::damage_flag>(projectile.damage_class), toucher);
        presenter.destroy_thing(toucher);
    }
    else if(touched_thing.type != flags::thing_type::Corpse && touched_thing.type != flags::thing_type::Item) {
        if(proj_flags & flags::weapon_flag::explodes_on_surface) {
            maybe_if(projectile.explode, [&](thing_template_id explode) {
                    presenter.create_thing_at_thing(explode, toucher);
                });
        }
        else {
            maybe_if(projectile.create_thing, [&](thing_template_id create_thing) {
                    presenter.create_thing_at_thing(create_thing, toucher);
                });
        }

        presenter.damage_thing(touched_thing_id, projectile.damage, flag_set<flags::damage_flag>(projectile.damage_class), toucher);
        presenter.destroy_thing(toucher);
    }
}

void weapon_controller_aspect::touched_surface(thing_id tid, surface_id touched_surface_id) {
    auto& projectile = presenter.model->get_thing(tid);
    if(projectile.type != flags::thing_type::Weapon) {
        return;
    }

    flag_set<flags::weapon_flag> proj_flags(projectile.type_flags);

    const auto& touched_surface = at_id(presenter.model->surfaces, touched_surface_id);
    if(((touched_surface.flags & flags::surface_flag::MagSealed) && (proj_flags & flags::weapon_flag::ricochets_on_magsealed))
            || (proj_flags & flags::weapon_flag::ricochets_on_surface)) {
        if(dot(projectile.vel, touched_surface.normal) <= 0.0f) {
            // Reflect projectile
            proj_flags -= flags::weapon_flag::does_not_affect_parent;
            projectile.type_flags = static_cast<int>(proj_flags);
            projectile.vel -= 2.0f * dot(projectile.vel, touched_surface.normal) * touched_surface.normal;
            presenter.sound_presenter->play_sound_class(thing_id(tid), flags::sound_subclass_type::deflected);
        }
        return;
    }

    if(proj_flags & flags::weapon_flag::explodes_on_surface) {
        maybe_if(projectile.explode, [&](thing_template_id explode) {
                presenter.create_thing_at_thing(explode, tid);
            });
    }
    else {
        maybe_if(projectile.create_thing, [&](thing_template_id create_thing) {
                presenter.create_thing_at_thing(create_thing, tid);
            });
    }

    presenter.model->script_model.send_to_linked(
            cog::message_type::damaged,
            /* sender */ touched_surface_id,
            /* source */ tid,
            /* source type */ presenter.model->get_thing_source_type(tid),
            /* param0 */ projectile.damage,
            /* param1 */ static_cast<int>(projectile.damage_class));
    presenter.destroy_thing(tid);
}

weapon_controller_aspect::weapon_controller_aspect(component_system &cs,
                                                   level_presenter &presenter)
    : inner_join_aspect(cs), presenter(presenter) {

    created_delegate =
        cs.bus.add_handler<events::thing_created>([&](events::thing_created const &e) {
        if(e.tpl.type == flags::thing_type::Weapon) {
            cs.emplace_component<components::weapon>(entity_id(e.thing));
        }
    });

    touched_thing_delegate =
        cs.bus.add_handler<events::touched_thing>([this](events::touched_thing const &e) {
        touched_thing(e.toucher, e.touched);
    });

    touched_surface_delegate =
        cs.bus.add_handler<events::touched_surface>([this](events::touched_surface const &e) {
        touched_surface(e.toucher, e.touched);
    });

    return;
}

void weapon_controller_aspect::update(gorc::time t,
                                      entity_id,
                                      components::weapon&,
                                      components::thing &thing) {
    flag_set<flags::weapon_flag> weap_flags(thing.type_flags);

    if(weap_flags & flags::weapon_flag::uses_damage_decay) {
        float alpha = static_cast<float>(t.elapsed_as_seconds()) / (thing.time_alive - thing.timer);
        thing.damage = lerp(thing.damage, thing.min_damage, alpha);
    }
}
