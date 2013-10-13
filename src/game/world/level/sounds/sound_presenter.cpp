#include "sound_presenter.h"
#include "game/world/level/level_model.h"
#include "game/components.h"
#include "content/manager.h"
#include "sound_model.h"

Gorc::Game::World::Level::Sounds::SoundPresenter::SoundPresenter(Content::Manager& contentManager)
	: contentManager(contentManager), levelModel(nullptr), model(nullptr) {
	return;
}

void Gorc::Game::World::Level::Sounds::SoundPresenter::Start(LevelModel& levelModel, SoundModel& soundModel) {
	this->levelModel = &levelModel;
	this->model = &soundModel;
}

void Gorc::Game::World::Level::Sounds::SoundPresenter::Update(double dt) {
	// Update listener
	auto camera_position = levelModel->CameraPosition;
	auto listener_target = camera_position + levelModel->CameraLook;
	sf::Listener::setDirection(Math::Get<0>(listener_target), Math::Get<1>(listener_target), Math::Get<2>(listener_target));
	sf::Listener::setPosition(Math::Get<0>(camera_position), Math::Get<1>(camera_position), Math::Get<2>(camera_position));
	// TODO: Handle camera orientation (not properly implemented in SFML).
	const auto& player_sec = levelModel->Sectors[levelModel->CameraSector];
	SetAmbientSound(player_sec.AmbientSound, player_sec.AmbientSoundVolume);

	// Update sounds
	for(auto& sound : model->Sounds) {
		sound.Update(dt, *levelModel);
	}

	for(auto& sound : model->Sounds) {
		if(sound.GetExpired()) {
			sound.Stop();
			model->Sounds.Destroy(sound);
		}
	}

	model->AmbientMusic.Update(dt);
}

void Gorc::Game::World::Level::Sounds::SoundPresenter::SetAmbientSound(Content::Assets::Sound const* sound, float volume) {
	if(&sound->Buffer == model->AmbientSound.getBuffer() && model->AmbientSound.getStatus() != sf::Sound::Stopped) {
		model->AmbientSound.setVolume(volume * 100.0f);
	}
	else if(sound != nullptr) {
		model->AmbientSound.setBuffer(sound->Buffer);
		model->AmbientSound.setLoop(true);
		model->AmbientSound.setVolume(volume * 100.0f);
		model->AmbientSound.setPosition(0,0,0);
		model->AmbientSound.setRelativeToListener(true);
		model->AmbientSound.setAttenuation(0.0f);
		model->AmbientSound.play();
	}
	else {
		model->AmbientSound.stop();
	}
}

// Verbs:

void Gorc::Game::World::Level::Sounds::SoundPresenter::ChangeSoundPitch(int channel, float pitch, float delay) {
	Sound& sound = model->Sounds[channel];
	sound.SetPitch(pitch, delay);
}

void Gorc::Game::World::Level::Sounds::SoundPresenter::ChangeSoundVol(int channel, float volume, float delay) {
	Sound& sound = model->Sounds[channel];
	sound.SetVolume(volume, delay);
}

void Gorc::Game::World::Level::Sounds::SoundPresenter::PlaySong(int start, int end, int loopto) {
	model->AmbientMusic.PlaySong(start, end, loopto);
}

int Gorc::Game::World::Level::Sounds::SoundPresenter::PlaySoundClass(int thing,
		Flags::SoundSubclassType subclass_type) {
	Thing& referenced_thing = levelModel->Things[thing];
	if(referenced_thing.SoundClass) {
		const Content::Assets::SoundSubclass& subclass = referenced_thing.SoundClass->Get(subclass_type);
		if(subclass.sound >= 0) {
			return PlaySoundThing(subclass.sound, thing, subclass.max_volume, subclass.min_radius, subclass.max_radius,
					subclass.flags + Flags::SoundFlag::ThingOriginMovesWithThing);
		}
	}

	return -1;
}

void Gorc::Game::World::Level::Sounds::SoundPresenter::PlayFoleyLoopClass(int thing, Flags::SoundSubclassType subclass_type) {
	Thing& referenced_thing = levelModel->Things[thing];

	if(referenced_thing.CurrentFoleyLoopChannel >= 0) {
		model->Sounds[referenced_thing.CurrentFoleyLoopChannel].Stop();
	}

	auto channel = PlaySoundClass(thing, subclass_type);
	if(channel >= 0) {
		referenced_thing.CurrentFoleyLoopChannel = channel;
	}
}

void Gorc::Game::World::Level::Sounds::SoundPresenter::StopFoleyLoop(int thing) {
	Thing& referenced_thing = levelModel->Things[thing];

	if(referenced_thing.CurrentFoleyLoopChannel >= 0) {
		model->Sounds[referenced_thing.CurrentFoleyLoopChannel].Stop();
	}
}

int Gorc::Game::World::Level::Sounds::SoundPresenter::PlaySoundLocal(int wav, float volume, float panning, FlagSet<Flags::SoundFlag> flags) {
	if(wav < 0) {
		return -1;
	}

	auto& snd = model->Sounds.Create();

	snd.PlaySoundLocal(contentManager.GetAsset<Content::Assets::Sound>(wav), volume, panning, flags);

	return snd.GetId();
}

int Gorc::Game::World::Level::Sounds::SoundPresenter::PlaySoundPos(int wav, Math::Vector<3> pos,
		float volume, float minrad, float maxrad, FlagSet<Flags::SoundFlag> flags) {
	if(wav < 0) {
		return -1;
	}

	auto& snd = model->Sounds.Create();

	snd.PlaySoundPos(contentManager.GetAsset<Content::Assets::Sound>(wav), pos, volume, minrad, maxrad, flags);

	return snd.GetId();
}

int Gorc::Game::World::Level::Sounds::SoundPresenter::PlaySoundThing(int wav, int thing, float volume, float minrad, float maxrad,
		FlagSet<Flags::SoundFlag> flags) {
	if(wav < 0) {
		return -1;
	}

	auto& snd = model->Sounds.Create();

	snd.PlaySoundThing(*levelModel, contentManager.GetAsset<Content::Assets::Sound>(wav), thing, volume, minrad, maxrad, flags);

	return snd.GetId();
}

void Gorc::Game::World::Level::Sounds::SoundPresenter::SetMusicVol(float volume) {
	model->AmbientMusic.SetVolume(volume);
}

void Gorc::Game::World::Level::Sounds::SoundPresenter::StopSound(int channel, float delay) {
	if(channel >= 0) {
		Sound& sound = model->Sounds[channel];
		sound.Stop(delay);
		model->Sounds.Destroy(channel);
	}
}

void Gorc::Game::World::Level::Sounds::SoundPresenter::RegisterVerbs(Cog::Verbs::VerbTable& verbTable, Components& components) {
	verbTable.AddVerb<void, 3>("changesoundpitch", [&components](int channel, float pitch, float delay) {
		components.CurrentLevelPresenter->SoundPresenter.ChangeSoundPitch(channel, pitch, delay);
	});

	verbTable.AddVerb<void, 3>("changesoundvol", [&components](int channel, float volume, float delay) {
		components.CurrentLevelPresenter->SoundPresenter.ChangeSoundVol(channel, volume, delay);
	});

	verbTable.AddVerb<void, 3>("playsong", [&components](int start, int end, int loopto) {
		components.CurrentLevelPresenter->SoundPresenter.PlaySong(start, end, loopto);
	});

	verbTable.AddVerb<int, 4>("playsoundlocal", [&components](int wav, float volume, float panning, int flags) {
		return components.CurrentLevelPresenter->SoundPresenter.PlaySoundLocal(wav, volume, panning, FlagSet<Flags::SoundFlag>(flags));
	});

	verbTable.AddVerb<int, 6>("playsoundpos", [&components](int wav, Math::Vector<3> pos, float volume, float min_rad, float max_rad, int flags) {
		return components.CurrentLevelPresenter->SoundPresenter.PlaySoundPos(wav, pos, volume, min_rad, max_rad, FlagSet<Flags::SoundFlag>(flags));
	});

	verbTable.AddVerb<int, 6>("playsoundthing", [&components](int wav, int thing, float volume, float min_rad, float max_rad, int flags) {
		return components.CurrentLevelPresenter->SoundPresenter.PlaySoundThing(wav, thing, volume, min_rad, max_rad, FlagSet<Flags::SoundFlag>(flags));
	});

	verbTable.AddVerb<void, 1>("setmusicvol", [&components](float vol) {
		components.CurrentLevelPresenter->SoundPresenter.SetMusicVol(vol);
	});

	verbTable.AddVerb<void, 2>("stopsound", [&components](int channel, float delay) {
		components.CurrentLevelPresenter->SoundPresenter.StopSound(channel, delay);
	});
}
