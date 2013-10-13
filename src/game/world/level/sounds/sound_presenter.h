#pragma once

#include "content/assets/sound.h"
#include "content/flags/sound_flag.h"
#include "content/flags/sound_subclass_type.h"
#include "framework/flag_set.h"
#include "framework/math/vector.h"

namespace Gorc {
namespace Content {
class Manager;
}

namespace Cog {
namespace Verbs {
class VerbTable;
}
}

namespace Game {
class Components;

namespace World {
namespace Level {
class LevelModel;
class Thing;

namespace Sounds {
class SoundModel;
class Sound;

class SoundPresenter {
private:
	Content::Manager& contentManager;
	LevelModel* levelModel;
	SoundModel* model;

public:
	SoundPresenter(Content::Manager&);

	void Start(LevelModel& levelModel, SoundModel& soundModel);
	void Update(double dt);

	void SetAmbientSound(Content::Assets::Sound const* sound, float volume);
	void PlayFoleyLoopClass(int thing, Flags::SoundSubclassType subclass);
	void StopFoleyLoop(int thing);

	// Sound verbs
	void ChangeSoundPitch(int channel, float pitch, float delay);
	void ChangeSoundVol(int channel, float volume, float delay);
	void PlaySong(int start, int end, int loopto);
	int PlaySoundClass(int thing, Flags::SoundSubclassType subclass);
	int PlaySoundLocal(int wav, float volume, float panning, FlagSet<Flags::SoundFlag> flags);
	int PlaySoundPos(int wav, Math::Vector<3> pos, float volume, float minrad, float maxrad, FlagSet<Flags::SoundFlag> flags);
	int PlaySoundThing(int wav, int thing, float volume, float minrad, float maxrad, FlagSet<Flags::SoundFlag> flags);
	void SetMusicVol(float volume);
	void StopSound(int channel, float delay);

	static void RegisterVerbs(Cog::Verbs::VerbTable&, Components&);
};

}
}
}
}
}
