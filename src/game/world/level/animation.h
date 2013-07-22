#pragma once

#include "framework/flagset.h"
#include "framework/math/vector.h"
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
	virtual void Stop();
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

class SlideSurfaceAnimation : public Animation {
private:
	LevelModel& model;
	unsigned int surface;
	Math::Vector<3> direction;

public:
	SlideSurfaceAnimation(LevelModel& model, unsigned int surface, const Math::Vector<3>& direction);

	void Update(double dt);
};

class CeilingSkyAnimation : public Animation {
private:
	LevelModel& model;
	Math::Vector<2> speed;

public:
	CeilingSkyAnimation(LevelModel& model, const Math::Vector<2>& speed);

	void Update(double dt);
};

}
}
}
}
