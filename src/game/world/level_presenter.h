#pragma once

#include "framework/place/presenter.h"
#include "framework/utility/time.h"
#include "level_place.h"

#include "game/flags/difficulty_mode.h"

#include "content/flags/jk_flag.h"
#include "content/flags/anim_flag.h"
#include "content/flags/armed_mode.h"
#include "content/flags/damage_flag.h"
#include "content/flags/ai_mode_flag.h"

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
namespace scripts { class script_presenter; }
namespace sounds { class sound_presenter; }
namespace gameplay {
    class thing_controller;
    class actor_controller;
    class player_controller;
    class cog_controller;
    class ghost_controller;
    class item_controller;
    class corpse_controller;
    class weapon_controller;
}

class level_model;
class thing;

class level_presenter : public gorc::place::presenter {
private:
    // Scratch space
    std::vector<std::tuple<int, int>> update_path_sector_scratch;

    level_state& components;
    level_place place;
    event::event_bus* event_bus;

    void initialize_world();

    void update_thing_sector(int thing_id, thing& thing, const vector<3>& oldThingPosition);

    gameplay::thing_controller& get_thing_controller(flags::thing_type type);

public:
    std::shared_ptr<content::content_manager> contentmanager;
    std::unique_ptr<level_model> model;

    std::unique_ptr<physics::physics_presenter> physics_presenter;
    std::unique_ptr<animations::animation_presenter> animation_presenter;
    std::unique_ptr<scripts::script_presenter> script_presenter;
    std::unique_ptr<sounds::sound_presenter> sound_presenter;
    std::unique_ptr<keys::key_presenter> key_presenter;
    std::unique_ptr<inventory::inventory_presenter> inventory_presenter;
    std::unique_ptr<camera::camera_presenter> camera_presenter;

    std::unique_ptr<gameplay::actor_controller> actor_controller;
    std::unique_ptr<gameplay::player_controller> player_controller;
    std::unique_ptr<gameplay::cog_controller> cog_controller;
    std::unique_ptr<gameplay::ghost_controller> ghost_controller;
    std::unique_ptr<gameplay::item_controller> item_controller;
    std::unique_ptr<gameplay::corpse_controller> corpse_controller;
    std::unique_ptr<gameplay::weapon_controller> weapon_controller;

    level_presenter(level_state& components, const level_place& place);
    ~level_presenter();

    void start(event::event_bus& eventBus);
    void update(const time& time);

    void translate_camera(const vector<3>& amt);
    void yaw_camera(double amt);
    void pitch_camera(double amt);
    void respawn();
    void jump();
    void activate();
    void damage();
    void fly();
    void crouch(bool is_crouched);

    void thing_sighted(int thing_id);

    // AI verbs
    void ai_clear_mode(int thing_id, flag_set<flags::ai_mode_flag> flags);
    flag_set<flags::ai_mode_flag> ai_get_mode(int thing_id);
    void ai_set_look_frame(int thing_id, int frame);
    void ai_set_look_pos(int thing_id, const vector<3>& pos);
    void ai_set_mode(int thing_id, flag_set<flags::ai_mode_flag> flags);
    void ai_set_move_frame(int thing_id, int frame);
    void ai_set_move_pos(int thing_id, const vector<3>& pos);
    void ai_set_move_speed(int thing_id, float speed);
    void ai_set_move_thing(int thing_id, int move_to_thing);

    // Color verbs
    void add_dynamic_tint(int player_id, const vector<3>& tint);

    // Creature verbs
    float get_thing_health(int thing_id);
    bool has_los(int look_thing_id, int target_thing_id);

    // Frame verbs
    int get_cur_frame(int thing_id);
    void jump_to_frame(int thing_id, int frame, int sector);
    void move_to_frame(int thing_id, int frame, float speed);
    void path_move_pause(int thing_id);
    void path_move_resume(int thing_id);
    void rotate_pivot(int thing_id, int frame, float time);

    // Level verbs
    float get_game_time();
    float get_level_time();
    void jk_end_level(bool success);

    // Misc verbs
    void jk_disable_saber(int thing_id);
    void jk_enable_saber(int thing_id, float damage, float collide_length, float unknown);
    void jk_set_saber_info(int thing_id, int side_mat, int tip_mat, float base_rad, float tip_rad, float length,
            int wall, int blood, int saber);
    void take_item(int thing_id, int player_id);

    // Player verbs
    int get_local_player_thing();

    // sector verbs
    void clear_sector_flags(int sector_id, flag_set<flags::sector_flag> flags);
    int first_thing_in_sector(int sector_id);
    flag_set<flags::sector_flag> get_sector_flags(int sector_id);
    int next_thing_in_sector(int thing_id);
    void sector_sound(int sector_id, int sound, float volume);
    void set_sector_adjoins(int sector_id, bool state);
    void set_sector_colormap(int sector_id, int colormap);
    void set_sector_flags(int sector_id, flag_set<flags::sector_flag> flags);
    void set_sector_light(int sector_id, float value, float delay);
    void set_sector_thrust(int sector_id, const vector<3>& thrust);
    void set_sector_tint(int sector_id, const vector<3>& color);

    // surface verbs
    void clear_adjoin_flags(int surface, flag_set<flags::adjoin_flag> flags);
    flags::geometry_mode get_face_geo_mode(int surface);
    vector<3> get_surface_center(int surface);
    int get_surface_sector(int surface);
    void set_adjoin_flags(int surface, flag_set<flags::adjoin_flag> flags);
    void set_face_geo_mode(int surface, flags::geometry_mode mode);
    void set_face_type(int surface, flag_set<flags::face_flag> flags);
    void set_surface_flags(int surface, flag_set<flags::surface_flag> flags);

    // System verbs
    int load_sound(const char* sound);

    // thing action verbs
    int create_thing(const content::assets::thing_template& tpl, unsigned int sector_num, const vector<3>& pos, const quaternion<float>& orientation);
    int create_thing(const std::string& tpl_name, unsigned int sector_num, const vector<3>& pos, const quaternion<float>& orientation);
    int create_thing(int tpl_id, unsigned int sector_num, const vector<3>& pos, const quaternion<float>& orientation);

    int fire_projectile(int parent_thing_id, int tpl_id, int fire_sound_id, int puppet_submode_id, const vector<3>& offset_vec, const vector<3>& err_vec,
            float extra, int projectile_flags, float autoaim_fovx, float autoaim_fovz);

    void attach_thing_to_thing(int thing_id, int base_id);
    int create_thing_at_thing(int tpl_id, int thing_id);
    float damage_thing(int thing_id, float damage, flag_set<flags::damage_flag> flags, int damager_id);
    void destroy_thing(int thing_id);
    void detach_thing(int thing_id);
    vector<3> get_thing_pos(int thing_id);
    void heal_thing(int thing_id, float health);
    bool is_thing_moving(int thing_id);

    void adjust_thing_pos(int thing_id, const vector<3>& new_pos);
    void set_thing_pos(int thing_id, const vector<3>& new_pos, const quaternion<float>& new_orient, int new_sector);
    vector<3> get_thing_lvec(int thing_id);

    // thing flag verbs
    void clear_actor_flags(int thing_id, flag_set<flags::actor_flag> flags);
    flag_set<flags::actor_flag> get_actor_flags(int thing_id);
    void set_actor_flags(int thing_id, flag_set<flags::actor_flag> flags);

    void clear_thing_flags(int thing_id, flag_set<flags::thing_flag> flags);
    flag_set<flags::thing_flag> get_thing_flags(int thing_id);
    void set_thing_flags(int thing_id, flag_set<flags::thing_flag> flags);

    void clear_thing_attach_flags(int thing_id, flag_set<flags::attach_flag> flags);
    flag_set<flags::attach_flag> get_thing_attach_flags(int thing_id);
    void set_thing_attach_flags(int thing_id, flag_set<flags::attach_flag> flags);

    void jk_clear_flags(int thing_id, flag_set<flags::jk_flag> flags);
    flag_set<flags::jk_flag> jk_get_flags(int thing_id);
    void jk_set_flags(int thing_id, flag_set<flags::jk_flag> flags);

    // thing property verbs
    int get_thing_parent(int thing_id);
    int get_thing_sector(int thing_id);
    flags::thing_type get_thing_type(int thing_id);
    void set_thing_type(int thing_id, flags::thing_type type);
    void set_thing_light(int thing_id, float light, float fade_time);

    // velocity verbs
    void stop_thing(int thing);

    // weapon verbs
    void jk_set_weapon_mesh(int player, int model);
    void set_armed_mode(int player, flags::armed_mode mode);
    flags::puppet_mode_type get_major_mode(int player);

    static void register_verbs(cog::verbs::verb_table&, level_state&);
};

}
}
}
