#include "sound.h"
#include "game/world/level/levelmodel.h"

void Gorc::Game::World::Level::Sounds::Sound::PlayAmbient(const Content::Assets::Sound& buffer, float volume, float panning, FlagSet<Flags::SoundFlag> flags) {
	expired = false;
	update_position = false;

	sound.setBuffer(buffer.Buffer);
	sound.setPosition(panning, 0.0f, 0.0f);
	sound.setRelativeToListener(true);
	sound.setVolume(volume * 100.0f);
	sound.setLoop(flags & Flags::SoundFlag::Loops);
	sound.setPitch(1.0f);
	sound.play();
}

void Gorc::Game::World::Level::Sounds::Sound::PlayVoice(const Content::Assets::Sound& buffer, float volume, FlagSet<Flags::SoundFlag> flags) {
	expired = false;
	update_position = false;

	sound.setBuffer(buffer.Buffer);
	sound.setPosition(0.0f, 0.0f, 1.0f);
	sound.setRelativeToListener(true);
	sound.setVolume(volume * 100.0f);
	sound.setLoop(flags & Flags::SoundFlag::Loops);
	sound.setPitch(1.0f);
	sound.play();
}

void Gorc::Game::World::Level::Sounds::Sound::PlayPositional(const Content::Assets::Sound& buffer, const Math::Vector<3>& position,
		float volume, float minrad, float maxrad, FlagSet<Flags::SoundFlag> flags) {
	expired = false;

	// TODO: Handle ambient flag.

	float actual_min_rad = std::min(minrad, maxrad);
	float actual_max_rad = std::max(minrad, maxrad);

	sound.setBuffer(buffer.Buffer);
	sound.setPosition(Math::Get<0>(position), Math::Get<1>(position), Math::Get<2>(position));
	sound.setRelativeToListener(false);
	sound.setVolume(volume * 100.0f);
	sound.setLoop(flags & Flags::SoundFlag::Loops);
	sound.setMinDistance(actual_min_rad);
	sound.setAttenuation(2.5f);
	sound.setPitch(1.0f);
	sound.play();
}

void Gorc::Game::World::Level::Sounds::Sound::PlaySoundLocal(const Content::Assets::Sound& sound, float volume, float panning,
		FlagSet<Flags::SoundFlag> flags) {
	if(flags & Flags::SoundFlag::Voice) {
		PlayVoice(sound, volume, flags);
	}
	else {
		PlayAmbient(sound, volume, panning, flags);
	}
}

void Gorc::Game::World::Level::Sounds::Sound::PlaySoundPos(const Content::Assets::Sound& sound, const Math::Vector<3>& pos, float volume,
		float minrad, float maxrad, FlagSet<Flags::SoundFlag> flags) {
	update_position = false;
	PlayPositional(sound, pos, volume, minrad, maxrad, flags);
}

void Gorc::Game::World::Level::Sounds::Sound::PlaySoundThing(const LevelModel& model, const Content::Assets::Sound& sound, int thing, float volume,
		float minrad, float maxrad, FlagSet<Flags::SoundFlag> flags) {
	this->thing = thing;
	update_position = (flags & Flags::SoundFlag::ThingOriginMovesWithThing);

	Math::Vector<3> pos = model.Things[thing].Position;
	PlayPositional(sound, pos, volume, minrad, maxrad, flags);
}

void Gorc::Game::World::Level::Sounds::Sound::SetPitch(float pitch, float delay) {
	// TODO: Implement delay
	sound.setPitch(pitch);
}

void Gorc::Game::World::Level::Sounds::Sound::SetVolume(float volume, float delay) {
	// TODO: Implement delay
	sound.setVolume(volume * 100.0f);
}

void Gorc::Game::World::Level::Sounds::Sound::Stop() {
	sound.stop();
}

void Gorc::Game::World::Level::Sounds::Sound::Stop(float delay) {
	// TODO: Implement delay
	sound.stop();
}

void Gorc::Game::World::Level::Sounds::Sound::Update(double dt, const LevelModel& model) {
	if(update_position) {
		Math::Vector<3> pos = model.Things[thing].Position;
		sound.setPosition(Math::Get<0>(pos), Math::Get<1>(pos), Math::Get<2>(pos));
	}

	expired = (sound.getStatus() != sf::Sound::Playing);
}
