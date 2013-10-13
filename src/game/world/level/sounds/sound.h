#pragma once

#include "content/flags/sound_flag.h"
#include "content/assets/sound.h"
#include "framework/flag_set.h"
#include "framework/math/vector.h"
#include <SFML/Audio.hpp>

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class LevelModel;
class Thing;

namespace Sounds {

class Sound {
private:
	sf::Sound sound;
	bool expired;

	int thing;
	bool update_position;

	void PlayAmbient(const Content::Assets::Sound& sound, float volume, float panning, FlagSet<Flags::SoundFlag> flags);
	void PlayVoice(const Content::Assets::Sound& sound, float volume, FlagSet<Flags::SoundFlag> flags);
	void PlayPositional(const Content::Assets::Sound& sound, const Math::Vector<3>& position, float volume, float minrad, float maxrad,
			FlagSet<Flags::SoundFlag> flags);

public:
	void PlaySoundLocal(const Content::Assets::Sound& sound, float volume, float panning, FlagSet<Flags::SoundFlag> flags);
	void PlaySoundPos(const Content::Assets::Sound& sound, const Math::Vector<3>& pos, float volume,
			float minrad, float maxrad, FlagSet<Flags::SoundFlag> flags);
	void PlaySoundThing(const LevelModel& model, const Content::Assets::Sound& sound, int thing, float volume,
			float minrad, float maxrad, FlagSet<Flags::SoundFlag> flags);

	void SetPitch(float pitch, float delay);
	void SetVolume(float volume, float delay);
	void Stop();
	void Stop(float delay);

	void Update(double dt, const LevelModel& model);

	inline bool GetExpired() {
		return expired;
	}
};

}
}
}
}
}
