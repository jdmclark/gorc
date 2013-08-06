#include "sound.h"
#include "game/world/level/levelmodel.h"

void Gorc::Game::World::Level::Sounds::Sound::PlayAmbient(const Content::Assets::Sound& buffer, float volume, float panning, FlagSet<Flags::SoundFlag> flags) {
	expired = false;
	update_position = false;

	sound.SetBuffer(buffer.Buffer);
	sound.SetPosition(panning, 0.0f, 0.0f);
	sound.SetRelativeToListener(true);
	sound.SetVolume(volume * 100.0f);
	sound.SetLoop(flags & Flags::SoundFlag::Loops);
	sound.SetPitch(1.0f);
	sound.Play();
}

void Gorc::Game::World::Level::Sounds::Sound::PlayVoice(const Content::Assets::Sound& buffer, float volume, FlagSet<Flags::SoundFlag> flags) {
	expired = false;
	update_position = false;

	sound.SetBuffer(buffer.Buffer);
	sound.SetPosition(0.0f, 0.0f, 1.0f);
	sound.SetRelativeToListener(true);
	sound.SetVolume(volume * 100.0f);
	sound.SetLoop(flags & Flags::SoundFlag::Loops);
	sound.SetPitch(1.0f);
	sound.Play();
}

void Gorc::Game::World::Level::Sounds::Sound::PlayPositional(const Content::Assets::Sound& buffer, const Math::Vector<3>& position,
		float volume, float minrad, float maxrad, FlagSet<Flags::SoundFlag> flags) {
	expired = false;

	// TODO: Handle ambient flag.

	float actual_min_rad = std::min(minrad, maxrad);
	float actual_max_rad = std::max(minrad, maxrad);

	sound.SetBuffer(buffer.Buffer);
	sound.SetPosition(Math::Get<0>(position), Math::Get<1>(position), Math::Get<2>(position));
	sound.SetRelativeToListener(false);
	sound.SetVolume(volume * 100.0f);
	sound.SetLoop(flags & Flags::SoundFlag::Loops);
	sound.SetMinDistance(actual_min_rad);
	sound.SetAttenuation(2.5f);
	sound.SetPitch(1.0f);
	sound.Play();
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

void Gorc::Game::World::Level::Sounds::Sound::PlaySoundThing(const LevelModel& model, const Content::Assets::Sound& sound, unsigned int thing, float volume,
		float minrad, float maxrad, FlagSet<Flags::SoundFlag> flags) {
	this->thing = thing;
	update_position = (flags & Flags::SoundFlag::ThingOriginMovesWithThing);

	Math::Vector<3> pos = model.Things[thing].Position;
	PlayPositional(sound, pos, volume, minrad, maxrad, flags);
}

void Gorc::Game::World::Level::Sounds::Sound::SetPitch(float pitch, float delay) {
	// TODO: Implement delay
	sound.SetPitch(pitch);
}

void Gorc::Game::World::Level::Sounds::Sound::SetVolume(float volume, float delay) {
	// TODO: Implement delay
	sound.SetVolume(volume * 100.0f);
}

void Gorc::Game::World::Level::Sounds::Sound::Stop() {
	sound.Stop();
}

void Gorc::Game::World::Level::Sounds::Sound::Stop(float delay) {
	// TODO: Implement delay
	sound.Stop();
}

void Gorc::Game::World::Level::Sounds::Sound::Update(double dt, const LevelModel& model) {
	if(update_position) {
		Math::Vector<3> pos = model.Things[thing].Position;
		sound.SetPosition(Math::Get<0>(pos), Math::Get<1>(pos), Math::Get<2>(pos));
	}

	expired = (sound.GetStatus() != sf::Sound::Playing);
}
