#include "sound.h"
#include "game/world/level/level_model.h"
#include "framework/math/util.h"

void gorc::game::world::level::sounds::sound::play_ambient(const content::assets::sound& buffer, float volume, float panning, flag_set<flags::sound_flag> flags) {
	expired = false;
	update_position = false;

	internal_sound.setBuffer(buffer.buffer);
	internal_sound.setPosition(panning, 0.0f, 0.0f);
	internal_sound.setRelativeToListener(true);
	internal_sound.setVolume(volume * 100.0f);
	internal_sound.setLoop(flags & flags::sound_flag::Loops);
	internal_sound.setPitch(1.0f);
	internal_sound.setAttenuation(0.0f);
	internal_sound.setMinDistance(1.0f);
	internal_sound.play();
}

void gorc::game::world::level::sounds::sound::play_voice(const content::assets::sound& buffer, float volume, flag_set<flags::sound_flag> flags) {
	expired = false;
	update_position = false;

	internal_sound.setBuffer(buffer.buffer);
	internal_sound.setPosition(0.0f, 0.0f, 1.0f);
	internal_sound.setRelativeToListener(true);
	internal_sound.setVolume(volume * 100.0f);
	internal_sound.setLoop(flags & flags::sound_flag::Loops);
	internal_sound.setPitch(1.0f);
	internal_sound.setAttenuation(0.0f);
	internal_sound.setMinDistance(1.0f);
	internal_sound.play();
}

void gorc::game::world::level::sounds::sound::play_positional(const content::assets::sound& buffer, const vector<3>& position,
		float volume, float minrad, float maxrad, flag_set<flags::sound_flag> flags) {
	expired = false;

	// TODO: Handle ambient flag.

	actual_min_rad = std::min(minrad, maxrad);
	actual_max_rad = std::max(minrad, maxrad);

	internal_sound.setBuffer(buffer.buffer);
	internal_sound.setPosition(get<0>(position), get<2>(position), get<1>(position));
	internal_sound.setRelativeToListener(false);
	internal_sound.setVolume(volume * 100.0f);
	internal_sound.setLoop(flags & flags::sound_flag::Loops);
	internal_sound.setMinDistance(actual_min_rad);
	internal_sound.setAttenuation(0.0f);
	internal_sound.setPitch(1.0f);
	internal_sound.play();
}

void gorc::game::world::level::sounds::sound::play_sound_local(const content::assets::sound& sound, float volume, float panning,
		flag_set<flags::sound_flag> flags) {
	if(flags & flags::sound_flag::Voice) {
		play_voice(sound, volume, flags);
	}
	else {
		play_ambient(sound, volume, panning, flags);
	}
}

void gorc::game::world::level::sounds::sound::play_sound_pos(const content::assets::sound& sound, const vector<3>& pos, float volume,
		float minrad, float maxrad, flag_set<flags::sound_flag> flags) {
	update_position = false;
	play_positional(sound, pos, volume, minrad, maxrad, flags);
}

void gorc::game::world::level::sounds::sound::play_sound_thing(const level_model& model, const content::assets::sound& sound, int thing, float volume,
		float minrad, float maxrad, flag_set<flags::sound_flag> flags) {
	this->thing = thing;
	update_position = (flags & flags::sound_flag::ThingOriginMovesWithThing);

	vector<3> pos = model.things[thing].position;
	play_positional(sound, pos, volume, minrad, maxrad, flags);
}

void gorc::game::world::level::sounds::sound::set_pitch(float pitch, float delay) {
	// TODO: Implement delay
	internal_sound.setPitch(pitch);
}

void gorc::game::world::level::sounds::sound::set_volume(float volume, float delay) {
	// TODO: Implement delay
	internal_sound.setVolume(volume * 100.0f);
}

void gorc::game::world::level::sounds::sound::stop() {
	internal_sound.stop();
}

void gorc::game::world::level::sounds::sound::stop(float delay) {
	// TODO: Implement delay
	internal_sound.stop();
}

void gorc::game::world::level::sounds::sound::update(double dt, const level_model& model) {
	if(update_position) {
		vector<3> pos = model.things[thing].position;
		internal_sound.setPosition(get<0>(pos), get<2>(pos), get<1>(pos));

		auto sound_dist = length(model.camera_position - pos);
		float attenuation = (actual_min_rad != actual_max_rad) ? ((sound_dist - actual_max_rad) / (actual_min_rad - actual_max_rad)) : 1.0f;
		attenuation = clamp(attenuation, 0.0f, 1.0f);

		internal_sound.setVolume(attenuation * 100.0f);
	}

	expired = (internal_sound.getStatus() != sf::Sound::Playing);
}

gorc::game::world::level::sounds::sound::~sound() {
	internal_sound.stop();
}
