#pragma once

#include "animation.h"
#include "content/flags/animflag.h"
#include "framework/flagset.h"
#include "framework/math/vector.h"
#include "framework/pool.h"

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

	Id<PoolPtr<Animation>> SurfaceAnim(int surface, float rate, FlagSet<Flags::AnimFlag> flags);
	Id<PoolPtr<Animation>> GetSurfaceAnim(int surface);

	void StopAnim(Id<PoolPtr<Animation>> anim);

	int GetSurfaceCel(int surface);
	void SetSurfaceCel(int surface, int cel);

	Id<PoolPtr<Animation>> SlideCeilingSky(float u_speed, float v_speed);
	Id<PoolPtr<Animation>> SlideSurface(int surface_id, const Math::Vector<3>& direction);

	Id<PoolPtr<Animation>> SurfaceLightAnim(int surface_id, float start_light, float end_light, float change_time);

	static void RegisterVerbs(Cog::Verbs::VerbTable&, Components&);
};

}
}
}
}
}
