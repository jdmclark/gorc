#include "sound_presenter.h"
#include "game/world/level_model.h"
#include "game/level_state.h"
#include "framework/content/manager.h"
#include "game/world/level_presenter.h"
#include "sound_model.h"

gorc::game::world::sounds::sound_presenter::sound_presenter(content::manager& contentmanager)
	: contentmanager(contentmanager), levelModel(nullptr), model(nullptr) {
	return;
}

void gorc::game::world::sounds::sound_presenter::start(level_model& levelModel, sound_model& soundModel) {
	this->levelModel = &levelModel;
	this->model = &soundModel;
}

void gorc::game::world::sounds::sound_presenter::update(const time& time) {
	// update listener
	const auto& cam = levelModel->camera_model.current_computed_state;
	auto camera_position = cam.position;
	auto listener_target = cam.look;
	sf::Listener::setDirection(get<0>(listener_target), get<2>(listener_target), -get<1>(listener_target));
	sf::Listener::setPosition(get<0>(camera_position), get<2>(camera_position), -get<1>(camera_position));
	// TODO: Handle camera orientation (not properly implemented in SFML).
	const auto& player_sec = levelModel->sectors[cam.containing_sector];
	set_ambient_sound(player_sec.ambient_sound, player_sec.ambient_sound_volume);

	// Update sounds
	for(auto& sound : model->sounds) {
		sound.update(time, *levelModel);
	}

	for(auto& sound : model->sounds) {
		if(sound.get_expired()) {
			model->sounds.erase(sound);
		}
	}

	model->ambient_music.update(time);
}

void gorc::game::world::sounds::sound_presenter::expunge_thing_sounds(int thing_id) {
	stop_foley_loop(thing_id);

	for(auto& sound : model->sounds) {
		if(sound.is_attached_to_thing(thing_id)) {
			sound.stop();
		}
	}

	update(time(0, 0.0));
}

void gorc::game::world::sounds::sound_presenter::set_ambient_sound(content::assets::sound const* sound, float volume) {
	if(&sound->buffer == model->ambient_sound.getBuffer() && model->ambient_sound.getStatus() != sf::Sound::Stopped) {
		model->ambient_sound.setVolume(volume * 100.0f);
	}
	else if(sound != nullptr) {
		model->ambient_sound.setBuffer(sound->buffer);
		model->ambient_sound.setLoop(true);
		model->ambient_sound.setVolume(volume * 100.0f);
		model->ambient_sound.setPosition(0,0,0);
		model->ambient_sound.setRelativeToListener(true);
		model->ambient_sound.setAttenuation(0.0f);
		model->ambient_sound.play();
	}
	else {
		model->ambient_sound.stop();
	}
}

// verbs:

void gorc::game::world::sounds::sound_presenter::change_sound_pitch(int channel, float pitch, float delay) {
	sound& sound = model->sounds[channel];
	sound.set_pitch(pitch, delay);
}

void gorc::game::world::sounds::sound_presenter::change_sound_vol(int channel, float volume, float delay) {
	sound& sound = model->sounds[channel];
	sound.set_volume(volume, delay);
}

void gorc::game::world::sounds::sound_presenter::play_song(int start, int end, int loopto) {
	model->ambient_music.play_song(start, end, loopto);
}

int gorc::game::world::sounds::sound_presenter::play_sound_class(int thing_id,
		flags::sound_subclass_type subclass_type) {
	thing& referenced_thing = levelModel->things[thing_id];
	if(referenced_thing.sound_class) {
		const content::assets::sound_subclass& subclass = referenced_thing.sound_class->get(subclass_type);
		if(subclass.sound >= 0) {
			return play_sound_thing(subclass.sound, thing_id, subclass.max_volume, subclass.min_radius, subclass.max_radius,
					subclass.flags + flags::sound_flag::ThingOriginMovesWithThing);
		}
	}

	return -1;
}

void gorc::game::world::sounds::sound_presenter::play_foley_loop_class(int thing_id, flags::sound_subclass_type subclass_type) {
	stop_foley_loop(thing_id);

	thing& referenced_thing = levelModel->things[thing_id];

	auto channel = play_sound_class(thing_id, subclass_type);
	if(channel >= 0) {
		referenced_thing.current_foley_loop_channel = &model->sounds[channel];
	}
}

void gorc::game::world::sounds::sound_presenter::stop_foley_loop(int thing_id) {
	thing& referenced_thing = levelModel->things[thing_id];

	if_set(referenced_thing.current_foley_loop_channel, then_do, [&referenced_thing](sound& snd) {
		snd.stop();
		referenced_thing.current_foley_loop_channel = nothing;
	});
}

int gorc::game::world::sounds::sound_presenter::play_sound_local(int wav, float volume, float panning, flag_set<flags::sound_flag> flags) {
	if(wav < 0) {
		return -1;
	}

	auto& snd = model->sounds.emplace();

	snd.play_sound_local(*levelModel, contentmanager.get_asset<content::assets::sound>(wav), volume, panning, flags);

	return snd.get_id();
}

int gorc::game::world::sounds::sound_presenter::play_sound_pos(int wav, vector<3> pos,
		float volume, float minrad, float maxrad, flag_set<flags::sound_flag> flags) {
	if(wav < 0) {
		return -1;
	}

	auto& snd = model->sounds.emplace();

	snd.play_sound_pos(*levelModel, contentmanager.get_asset<content::assets::sound>(wav), pos, volume, minrad, maxrad, flags);

	return snd.get_id();
}

int gorc::game::world::sounds::sound_presenter::play_sound_thing(int wav, int thing, float volume, float minrad, float maxrad,
		flag_set<flags::sound_flag> flags) {
	if(wav < 0) {
		return -1;
	}

	auto& snd = model->sounds.emplace();

	snd.play_sound_thing(*levelModel, contentmanager.get_asset<content::assets::sound>(wav), thing, volume, minrad, maxrad, flags);

	return snd.get_id();
}

void gorc::game::world::sounds::sound_presenter::set_music_vol(float volume) {
	model->ambient_music.set_volume(volume);
}

void gorc::game::world::sounds::sound_presenter::stop_sound(int channel, float delay) {
	if(channel >= 0) {
		sound& sound = model->sounds[channel];
		sound.stop(delay);
	}
}

void gorc::game::world::sounds::sound_presenter::register_verbs(cog::verbs::verb_table& verbTable, level_state& components) {
	verbTable.add_verb<void, 3>("changesoundpitch", [&components](int channel, float pitch, float delay) {
		components.current_level_presenter->sound_presenter->change_sound_pitch(channel, pitch, delay);
	});

	verbTable.add_verb<void, 3>("changesoundvol", [&components](int channel, float volume, float delay) {
		components.current_level_presenter->sound_presenter->change_sound_vol(channel, volume, delay);
	});

	verbTable.add_verb<void, 3>("playsong", [&components](int start, int end, int loopto) {
		components.current_level_presenter->sound_presenter->play_song(start, end, loopto);
	});

	verbTable.add_verb<int, 4>("playsoundlocal", [&components](int wav, float volume, float panning, int flags) {
		return components.current_level_presenter->sound_presenter->play_sound_local(wav, volume, panning, flag_set<flags::sound_flag>(flags));
	});

	verbTable.add_verb<int, 6>("playsoundpos", [&components](int wav, vector<3> pos, float volume, float min_rad, float max_rad, int flags) {
		return components.current_level_presenter->sound_presenter->play_sound_pos(wav, pos, volume, min_rad, max_rad, flag_set<flags::sound_flag>(flags));
	});

	verbTable.add_verb<int, 6>("playsoundthing", [&components](int wav, int thing, float volume, float min_rad, float max_rad, int flags) {
		return components.current_level_presenter->sound_presenter->play_sound_thing(wav, thing, volume, min_rad, max_rad, flag_set<flags::sound_flag>(flags));
	});

	verbTable.add_verb<void, 1>("setmusicvol", [&components](float vol) {
		components.current_level_presenter->sound_presenter->set_music_vol(vol);
	});

	verbTable.add_verb<void, 2>("stopsound", [&components](int channel, float delay) {
		components.current_level_presenter->sound_presenter->stop_sound(channel, delay);
	});
}
