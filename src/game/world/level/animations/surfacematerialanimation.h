#pragma once

#include "animation.h"
#include "content/flags/animflag.h"
#include "framework/flagset.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class LevelModel;

namespace Animations {

class SurfaceMaterialAnimation : public Animation {
private:
	LevelModel& model;
	unsigned int surface;
	double framerate;
	FlagSet<Flags::AnimFlag> flag;
	double framerate_accumulator = 0.0;
	int num_cels;

public:
	SurfaceMaterialAnimation(LevelModel& model, unsigned int surface, double framerate, FlagSet<Flags::AnimFlag> flag, int anim_num);

	void Update(double dt);
	void Stop();
};

}
}
}
}
}
