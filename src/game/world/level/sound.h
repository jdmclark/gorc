#pragma once

#include "content/assets/flags.h"
#include "content/assets/sound.h"
#include "framework/math/vector.h"
#include <SFML/Audio.hpp>

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class LevelModel;

class Sound {
private:
	sf::Sound sound;
	bool expired;

	unsigned int thing;
	bool update_position;

	void PlayAmbient(const Content::Assets::Sound& sound, float volume, float panning, FlagSet<Content::Assets::SoundFlag> flags);
	void PlayVoice(const Content::Assets::Sound& sound, float volume, FlagSet<Content::Assets::SoundFlag> flags);
	void PlayPositional(const Content::Assets::Sound& sound, const Math::Vector<3>& position, float volume, float minrad, float maxrad,
			FlagSet<Content::Assets::SoundFlag> flags);

public:
	void PlaySoundLocal(const Content::Assets::Sound& sound, float volume, float panning, FlagSet<Content::Assets::SoundFlag> flags);
	void PlaySoundPos(const Content::Assets::Sound& sound, const Math::Vector<3>& pos, float volume,
			float minrad, float maxrad, FlagSet<Content::Assets::SoundFlag> flags);
	void PlaySoundThing(const LevelModel& model, const Content::Assets::Sound& sound, unsigned int thing, float volume,
			float minrad, float maxrad, FlagSet<Content::Assets::SoundFlag> flags);

	void Update(double dt, const LevelModel& model);

	inline bool GetExpired() {
		return expired;
	}
};

}
}
}
}
