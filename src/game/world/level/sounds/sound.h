#pragma once

#include "content/flags/sound_flag.h"
#include "content/assets/sound.h"
#include "framework/utility/flag_set.h"
#include "framework/math/vector.h"
#include <SFML/Audio.hpp>

namespace gorc {
namespace game {
namespace world {
namespace level {

class level_model;
class thing;

namespace sounds {

class sound {
private:
	sf::Sound internal_sound;
	bool expired;

	int thing;
	bool update_position;
	float actual_min_rad;
	float actual_max_rad;

	void play_ambient(const content::assets::sound& sound, float volume, float panning, flag_set<flags::sound_flag> flags);
	void play_voice(const content::assets::sound& sound, float volume, flag_set<flags::sound_flag> flags);
	void play_positional(const content::assets::sound& sound, const vector<3>& position, float volume, float minrad, float maxrad,
			flag_set<flags::sound_flag> flags);

public:
	void play_sound_local(const level_model& model, const content::assets::sound& sound, float volume, float panning, flag_set<flags::sound_flag> flags);
	void play_sound_pos(const level_model& model, const content::assets::sound& sound, const vector<3>& pos, float volume,
			float minrad, float maxrad, flag_set<flags::sound_flag> flags);
	void play_sound_thing(const level_model& model, const content::assets::sound& sound, int thing, float volume,
			float minrad, float maxrad, flag_set<flags::sound_flag> flags);

	void set_pitch(float pitch, float delay);
	void set_volume(float volume, float delay);
	void stop();
	void stop(float delay);

	void update(double dt, const level_model& model);

	inline bool get_expired() {
		return expired;
	}

	~sound();
};

}
}
}
}
}
