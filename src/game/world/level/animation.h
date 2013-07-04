#pragma once

#include "framework/flagset.h"
#include "content/assets/flags.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class LevelModel;

class Animation {
public:
	bool Expired = false;

	virtual ~Animation();

	virtual void Update(double dt) = 0;
	virtual void Stop() = 0;
};

class SurfaceAnimation : public Animation {
private:
	LevelModel& model;
	unsigned int surface;
	double framerate;
	FlagSet<Content::Assets::SurfaceAnimationFlag> flag;
	double framerate_accumulator = 0.0;
	int num_cels;

public:
	SurfaceAnimation(LevelModel& model, unsigned int Surface, double framerate, FlagSet<Content::Assets::SurfaceAnimationFlag> flag, int anim_num);

	void Update(double dt);
	void Stop();
};

}
}
}
}
