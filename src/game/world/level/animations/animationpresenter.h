#pragma once

#include "content/flags/animflag.h"
#include "framework/flagset.h"
#include "framework/math/vector.h"

namespace Gorc {

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

namespace Animations {

class AnimationModel;

class AnimationPresenter {
private:
	LevelModel* levelModel;
	AnimationModel* model;

public:
	void Start(LevelModel& levelModel, AnimationModel& model);
	void Update(double dt);

	int SurfaceAnim(int surface, float rate, FlagSet<Flags::AnimFlag> flags);
	int GetSurfaceAnim(int surface);

	void StopAnim(int anim);

	int GetSurfaceCel(int surface);
	void SetSurfaceCel(int surface, int cel);

	int SlideCeilingSky(float u_speed, float v_speed);
	int SlideSurface(int surface_id, const Math::Vector<3>& direction);

	static void RegisterVerbs(Cog::Verbs::VerbTable&, Components&);
};

}
}
}
}
}
