#pragma once

#include "thingcontroller.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class Thing;

namespace Gameplay {

class FilteredClosestRayResultCallback;

class CharacterController : public ThingController {
private:
	void RunFallingSweep(unsigned int thing_id, Thing& thing, double dt, FilteredClosestRayResultCallback& rrcb);
	void RunWalkingSweep(unsigned int thing_id, Thing& thing, double dt, FilteredClosestRayResultCallback& rrcb);
	void UpdateFalling(unsigned int thing_id, Thing& thing, double dt);
	void UpdateStandingOnSurface(unsigned int thing_id, Thing& thing, double dt);
	void UpdateStandingOnThing(unsigned int thing_id, Thing& thing, double dt);
	void StepOnSurface(unsigned int thing_id, Thing& thing, unsigned int surf_id, const FilteredClosestRayResultCallback& rrcb);
	void StepOnThing(unsigned int thing_id, Thing& thing, unsigned int step_thing_id, const FilteredClosestRayResultCallback& rrcb);
	void LandOnSurface(unsigned int thing_id, Thing& thing, unsigned int surf_id, const FilteredClosestRayResultCallback& rrcb);
	void LandOnThing(unsigned int thing_id, Thing& thing, unsigned int land_thing_id, const FilteredClosestRayResultCallback& rrcb);
	void Jump(unsigned int thing_id, Thing& thing);
	void JumpFromSurface(unsigned int thing_id, Thing& thing, unsigned int surf_id);
	void JumpFromThing(unsigned int thing_id, Thing& thing, unsigned int jump_thing_id);

public:
	using ThingController::ThingController;

	virtual void Update(unsigned int thing_id, double dt) override;
	virtual unsigned int Create(const Content::Assets::Template& tpl, unsigned int sector_id,
				const Math::Vector<3>& pos, const Math::Vector<3>& orient) override;
};

}
}
}
}
}
