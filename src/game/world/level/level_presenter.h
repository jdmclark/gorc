#pragma once

#include "framework/place/presenter.h"
#include "game/components.h"
#include "level_place.h"
#include "cog/vm/virtual_machine.h"
#include "sounds/music.h"
#include "content/flags/anim_flag.h"
#include "content/flags/damage_flag.h"
#include "content/assets/inventory.h"
#include "physics/shape.h"
#include "game/flags/difficulty_mode.h"
#include "game/world/level/animations/animation_presenter.h"
#include "game/world/level/scripts/script_presenter.h"
#include "game/world/level/sounds/sound_presenter.h"
#include "game/world/level/keys/key_presenter.h"
#include "game/world/level/gameplay/inventory_presenter.h"
#include "game/world/level/gameplay/actor_controller.h"
#include "game/world/level/gameplay/player_controller.h"
#include "game/world/level/gameplay/cog_controller.h"
#include "game/world/level/gameplay/ghost_controller.h"
#include "game/world/level/gameplay/item_controller.h"
#include "game/world/level/gameplay/corpse_controller.h"
#include "game/world/level/gameplay/weapon_controller.h"

#include <memory>
#include <stack>

namespace gorc {
namespace game {
namespace world {
namespace level {

class level_model;
class thing;

class level_presenter : public gorc::place::presenter {
private:
	// Scratch space
	std::vector<std::tuple<unsigned int, unsigned int>> update_path_sector_scratch;

	components& components;
	level_place place;

	void initialize_world();
	void physics_tick_update(double dt);

	bool point_inside_sector(const vector<3>& position, const content::assets::level_sector& sec);
	bool point_path_passes_through_adjoin(const physics::segment& segment,
			const content::assets::level_sector& sec, const content::assets::level_surface& surf);
	bool update_path_sector(const physics::segment& segment, const content::assets::level_sector& sector,
			std::vector<std::tuple<unsigned int, unsigned int>>& path);

	void update_thing_sector(int thing_id, thing& thing, const vector<3>& oldThingPosition);
	void update_camera();

	gameplay::thing_controller& get_thing_controller(flags::thing_type type);

	bool need_respawn = false;
	void do_respawn();

public:
	std::unique_ptr<level_model> model;

	animations::animation_presenter animation_presenter;
	scripts::script_presenter script_presenter;
	sounds::sound_presenter sound_presenter;
	keys::key_presenter key_presenter;
	gameplay::inventory_presenter inventory_presenter;

	gameplay::actor_controller actor_controller;
	gameplay::player_controller player_controller;
	gameplay::cog_controller cog_controller;
	gameplay::ghost_controller ghost_controller;
	gameplay::item_controller item_controller;
	gameplay::corpse_controller corpse_controller;
	gameplay::weapon_controller weapon_controller;

	level_presenter(class components& components, const level_place& place);

	void start(event::event_bus& eventBus);
	void update(double dt);

	void translate_camera(const vector<3>& amt);
	void yaw_camera(double amt);
	void pitch_camera(double amt);
	void respawn();
	void jump();
	void activate();
	void damage();

	void thing_sighted(int thing_id);

	// Color verbs
	void add_dynamic_tint(int player_id, const vector<3>& tint);

	// Frame verbs
	int get_cur_frame(int thing_id);
	void jump_to_frame(int thing_id, int frame, int sector);
	void MoveToFrame(int thing_id, int frame, float speed);

	// level verbs
	float get_game_time();
	float get_level_time();

	// Misc verbs
	void take_item(int thing_id, int player_id);

	// Player verbs
	int get_local_player_thing();

	// sector verbs
	void set_sector_adjoins(int sector_id, bool state);
	void set_sector_light(int sector_id, float value, float delay);
	void set_sector_thrust(int sector_id, const vector<3>& thrust);
	void set_sector_tint(int sector_id, const vector<3>& color);

	// surface verbs
	void clear_adjoin_flags(int surface, flag_set<flags::adjoin_flag> flags);
	vector<3> get_surface_center(int surface);
	void set_adjoin_flags(int surface, flag_set<flags::adjoin_flag> flags);
	void set_face_geo_mode(int surface, flags::geometry_mode mode);
	void set_surface_flags(int surface, flag_set<flags::surface_flag> flags);

	// System verbs
	int load_sound(const char* sound);

	// thing action verbs
	int create_thing(const content::assets::thing_template& tpl, unsigned int sector_num, const vector<3>& pos, const vector<3>& orientation);
	int create_thing(const std::string& tpl_name, unsigned int sector_num, const vector<3>& pos, const vector<3>& orientation);
	int create_thing(int tpl_id, unsigned int sector_num, const vector<3>& pos, const vector<3>& orientation);

	void attach_thing_to_thing(int thing_id, int base_id);
	int create_thing_at_thing(int tpl_id, int thing_id);
	float damage_thing(int thing_id, float damage, flag_set<flags::DamageFlag> flags, int damager_id);
	void destroy_thing(int thing_id);
	void detach_thing(int thing_id);
	vector<3> get_thing_pos(int thing_id);
	bool is_thing_moving(int thing_id);

	void adjust_thing_pos(int thing_id, const vector<3>& new_pos);
	void set_thing_pos(int thing_id, const vector<3>& new_pos, const vector<3>& new_orient, int new_sector);

	// thing flag verbs
	void clear_actor_flags(int thing_id, flag_set<flags::actor_flag> flags);
	void clear_thing_flags(int thing_id, flag_set<flags::thing_flag> flags);

	flag_set<flags::actor_flag> get_actor_flags(int thing_id);

	void set_actor_flags(int thing_id, flag_set<flags::actor_flag> flags);

	// thing property verbs
	int get_thing_parent(int thing_id);
	int get_thing_sector(int thing_id);
	flags::thing_type get_thing_type(int thing_id);
	void set_thing_type(int thing_id, flags::thing_type type);
	void set_thing_light(int thing_id, float light, float fade_time);

	static void register_verbs(cog::verbs::verb_table&, class components&);
};

}
}
}
}
