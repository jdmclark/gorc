#pragma once

#include "libold/base/place/presenter.hpp"
#include "libold/base/utility/time.hpp"
#include "level_place.hpp"

#include "game/flags/difficulty_mode.hpp"

#include "libold/content/flags/jk_flag.hpp"
#include "libold/content/flags/anim_flag.hpp"
#include "libold/content/flags/armed_mode.hpp"
#include "libold/content/flags/damage_flag.hpp"
#include "libold/content/flags/ai_mode_flag.hpp"

#include "components/thing.hpp"
#include "libold/base/utility/entity_id.hpp"
#include "jk/cog/script/verb_table.hpp"

#include <memory>
#include <stack>
#include <set>
#include <unordered_map>

namespace gorc {
namespace game {
class level_state;

namespace world {

namespace animations { class animation_presenter; }
namespace camera { class camera_presenter; }
namespace inventory { class inventory_presenter; }
namespace keys { class key_presenter; }
namespace physics { class physics_presenter; }
namespace sounds { class sound_presenter; }

class level_model;

class level_presenter : public gorc::place::presenter {
private:
    // Scratch space
    std::vector<std::tuple<sector_id, surface_id>> update_path_sector_scratch;

    std::set<thing_id> things_to_destroy;

    level_state& components;
    level_place place;
    event_bus* eventbus;

    void initialize_world();

    void update_thing_sector(thing_id, components::thing& thing, const vector<3>& oldThingPosition);

public:
    std::shared_ptr<content_manager> contentmanager;
    std::unique_ptr<level_model> model;

    std::unique_ptr<physics::physics_presenter> physics_presenter;
    std::unique_ptr<animations::animation_presenter> animation_presenter;
    std::unique_ptr<sounds::sound_presenter> sound_presenter;
    std::unique_ptr<keys::key_presenter> key_presenter;
    std::unique_ptr<inventory::inventory_presenter> inventory_presenter;
    std::unique_ptr<camera::camera_presenter> camera_presenter;

    level_presenter(level_state& components, const level_place& place);
    ~level_presenter();

    void start(event_bus& eventBus);
    void update(const gorc::time& time);

    void translate_camera(const vector<3>& amt);
    void yaw_camera(double amt);
    void pitch_camera(double amt);
    void respawn();
    void jump();
    void activate();
    void damage();
    void fly();
    void crouch(bool is_crouched);

    void thing_sighted(thing_id);

    // AI verbs
    void ai_clear_mode(thing_id, flag_set<flags::ai_mode_flag> flags);
    flag_set<flags::ai_mode_flag> ai_get_mode(thing_id);
    void ai_set_look_frame(thing_id, int frame);
    void ai_set_look_pos(thing_id, const vector<3>& pos);
    void ai_set_mode(thing_id, flag_set<flags::ai_mode_flag> flags);
    void ai_set_move_frame(thing_id, int frame);
    void ai_set_move_pos(thing_id, const vector<3>& pos);
    void ai_set_move_speed(thing_id, float speed);
    void ai_set_move_thing(thing_id, thing_id move_to_thing);

    // Color verbs
    void add_dynamic_tint(thing_id, const color_rgb& tint);

    // Creature verbs
    float get_thing_health(thing_id);
    bool has_los(thing_id look_thing_id, thing_id target_thing_id);

    // Frame verbs
    int get_cur_frame(thing_id);
    void jump_to_frame(thing_id, int frame, sector_id sector);
    void move_to_frame(thing_id, int frame, float speed);
    void path_move_pause(thing_id);
    void path_move_resume(thing_id);
    void rotate_pivot(thing_id, int frame, float time);

    // Level verbs
    float get_game_time();
    float get_level_time();
    void jk_end_level(bool success);

    // Misc verbs
    void jk_disable_saber(thing_id);
    void jk_enable_saber(thing_id, float damage, float collide_length, float unknown);
    void jk_set_saber_info(thing_id, material_id side_mat, material_id tip_mat, float base_rad, float tip_rad, float length,
            thing_template_id wall, thing_template_id blood, thing_template_id saber);
    void take_item(thing_id taken, thing_id player_id);

    // Player verbs
    thing_id get_local_player_thing();

    // sector verbs
    void clear_sector_flags(sector_id, flag_set<flags::sector_flag> flags);
    thing_id first_thing_in_sector(sector_id);
    flag_set<flags::sector_flag> get_sector_flags(sector_id);
    thing_id next_thing_in_sector(thing_id);
    void sector_sound(sector_id, sound_id sound, float volume);
    void set_sector_adjoins(sector_id, bool state);
    void set_sector_colormap(sector_id, colormap_id colormap);
    void set_sector_flags(sector_id, flag_set<flags::sector_flag> flags);
    void set_sector_light(sector_id, float value, float delay);
    void set_sector_thrust(sector_id, const vector<3>& thrust);
    void set_sector_tint(sector_id, const vector<3>& color);

    // surface verbs
    void clear_adjoin_flags(surface_id surface, flag_set<flags::adjoin_flag> flags);
    flags::geometry_mode get_face_geo_mode(surface_id surface);
    vector<3> get_surface_center(surface_id surface);
    sector_id get_surface_sector(surface_id surface);
    void set_adjoin_flags(surface_id surface, flag_set<flags::adjoin_flag> flags);
    void set_face_geo_mode(surface_id surface, flags::geometry_mode mode);
    void set_face_type(surface_id surface, flag_set<flags::face_flag> flags);
    void set_surface_flags(surface_id surface, flag_set<flags::surface_flag> flags);

    // System verbs
    sound_id load_sound(const char* sound);

    // thing action verbs
    thing_id create_thing(const content::assets::thing_template& tpl, sector_id sector_num, const vector<3>& pos, const quaternion<float>& orientation);
    thing_id create_thing(const std::string& tpl_name, sector_id sector_num, const vector<3>& pos, const quaternion<float>& orientation);
    thing_id create_thing(thing_template_id tpl_id, sector_id sector_num, const vector<3>& pos, const quaternion<float>& orientation);

    thing_id fire_projectile(thing_id parent_thing_id, thing_template_id tpl_id, sound_id fire_sound_id, int puppet_submode_id, const vector<3>& offset_vec, const vector<3>& err_vec,
            float extra, int projectile_flags, float autoaim_fovx, float autoaim_fovz);

    void attach_thing_to_thing(thing_id attacher, thing_id base_id);
    thing_id create_thing_at_thing(thing_template_id tpl_id, thing_id);
    float damage_thing(thing_id, float damage, flag_set<flags::damage_flag> flags, thing_id damager_id);
    void destroy_thing(thing_id);
    void real_destroy_thing(thing_id);
    void detach_thing(thing_id);
    vector<3> get_thing_pos(thing_id);
    void heal_thing(thing_id, float health);
    bool is_thing_moving(thing_id);

    void adjust_thing_pos(thing_id, const vector<3>& new_pos);
    void set_thing_pos(thing_id, const vector<3>& new_pos, const quaternion<float>& new_orient, sector_id new_sector);
    vector<3> get_thing_lvec(thing_id);

    // thing flag verbs
    void clear_actor_flags(thing_id, flag_set<flags::actor_flag> flags);
    flag_set<flags::actor_flag> get_actor_flags(thing_id);
    void set_actor_flags(thing_id, flag_set<flags::actor_flag> flags);

    void clear_thing_flags(thing_id, flag_set<flags::thing_flag> flags);
    flag_set<flags::thing_flag> get_thing_flags(thing_id);
    void set_thing_flags(thing_id, flag_set<flags::thing_flag> flags);

    void clear_thing_attach_flags(thing_id, flag_set<flags::attach_flag> flags);
    flag_set<flags::attach_flag> get_thing_attach_flags(thing_id);
    void set_thing_attach_flags(thing_id, flag_set<flags::attach_flag> flags);

    void jk_clear_flags(thing_id, flag_set<flags::jk_flag> flags);
    flag_set<flags::jk_flag> jk_get_flags(thing_id);
    void jk_set_flags(thing_id, flag_set<flags::jk_flag> flags);

    // thing property verbs
    thing_id get_thing_parent(thing_id);
    sector_id get_thing_sector(thing_id);
    flags::thing_type get_thing_type(thing_id);
    void set_thing_light(thing_id, float light, float fade_time);

    // velocity verbs
    void stop_thing(thing_id);

    // weapon verbs
    void jk_set_weapon_mesh(thing_id player, model_id model);
    void set_armed_mode(thing_id player, flags::armed_mode mode);
    flags::puppet_mode_type get_major_mode(thing_id player);

    static void register_verbs(cog::verb_table&, level_state&);
};

}
}
}
