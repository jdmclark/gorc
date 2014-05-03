#include "weapon_controller.h"
#include "game/world/level_presenter.h"
#include "game/world/level_model.h"
#include "content/flags/weapon_flag.h"
#include "game/world/scripts/script_presenter.h"
#include "game/world/sounds/sound_presenter.h"

void gorc::game::world::gameplay::weapon_controller::touched_thing(int thing_id, int touched_thing_id) {
    thing_controller::touched_thing(thing_id, touched_thing_id);

    auto& projectile = presenter.model->get_thing(thing_id);
    flag_set<flags::weapon_flag> proj_flags(projectile.type_flags);

    auto& touched_thing = presenter.model->get_thing(touched_thing_id);

    if(projectile.parent_thing != -1 && projectile.parent_thing == touched_thing.parent_thing) {
        // Projectiles fired by same player. Ignore collision.
        return;
    }

    // Skip projectiles moving away from thing they were fired by.
    if(projectile.parent_thing == touched_thing_id && (proj_flags & flags::weapon_flag::does_not_affect_parent)) {
        return;
    }

    if(touched_thing.type == flags::thing_type::Actor || touched_thing.type == flags::thing_type::Player) {
        if(proj_flags & flags::weapon_flag::explodes_on_thing) {
            presenter.create_thing_at_thing(projectile.explode, thing_id);
        }
        else {
            presenter.create_thing_at_thing(projectile.flesh_hit, thing_id);
        }

        presenter.damage_thing(touched_thing_id, projectile.damage, flag_set<flags::damage_flag>(projectile.damage_class), thing_id);
        presenter.destroy_thing(thing_id);
    }
    else if(touched_thing.type != flags::thing_type::Corpse && touched_thing.type != flags::thing_type::Item) {
        if(proj_flags & flags::weapon_flag::explodes_on_surface) {
            presenter.create_thing_at_thing(projectile.explode, thing_id);
        }
        else {
            presenter.create_thing_at_thing(projectile.create_thing, thing_id);
        }

        presenter.damage_thing(touched_thing_id, projectile.damage, flag_set<flags::damage_flag>(projectile.damage_class), thing_id);
        presenter.destroy_thing(thing_id);
    }
}

void gorc::game::world::gameplay::weapon_controller::touched_surface(int thing_id, int touched_surface_id) {
    thing_controller::touched_surface(thing_id, touched_surface_id);

    auto& projectile = presenter.model->get_thing(thing_id);
    flag_set<flags::weapon_flag> proj_flags(projectile.type_flags);

    const auto& touched_surface = presenter.model->surfaces[touched_surface_id];
    if(((touched_surface.flags & flags::surface_flag::MagSealed) && (proj_flags & flags::weapon_flag::ricochets_on_magsealed))
            || (proj_flags & flags::weapon_flag::ricochets_on_surface)) {
        if(dot(projectile.vel, touched_surface.normal) <= 0.0f) {
            // Reflect projectile
            proj_flags -= flags::weapon_flag::does_not_affect_parent;
            projectile.type_flags = static_cast<int>(proj_flags);
            projectile.vel -= 2.0f * dot(projectile.vel, touched_surface.normal) * touched_surface.normal;
            presenter.sound_presenter->play_sound_class(thing_id, flags::sound_subclass_type::deflected);
        }
        return;
    }

    if(proj_flags & flags::weapon_flag::explodes_on_surface) {
        presenter.create_thing_at_thing(projectile.explode, thing_id);
    }
    else {
        presenter.create_thing_at_thing(projectile.create_thing, thing_id);
    }

    presenter.script_presenter->send_message_to_linked(cog::message_id::damaged,
            touched_surface_id, flags::message_type::surface,
            thing_id, flags::message_type::thing,
            projectile.damage, static_cast<int>(projectile.damage_class));
    presenter.destroy_thing(thing_id);
}

void gorc::game::world::gameplay::weapon_controller::update(int thing_id, double dt) {
    auto& thing = presenter.model->get_thing(thing_id);
    flag_set<flags::weapon_flag> weap_flags(thing.type_flags);

    if(weap_flags & flags::weapon_flag::uses_damage_decay) {
        float alpha = static_cast<float>(dt) / (thing.time_alive - thing.timer);
        thing.damage = lerp(thing.damage, thing.min_damage, alpha);
    }

    thing_controller::update(thing_id, dt);
}
