#pragma once

#include "thingcontroller.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class Thing;

namespace Gameplay {

enum class StandingMaterial {
	None,
	Hard,
	Dirt,
	Metal,
	ShallowWater,
	DeepWater,
	VeryDeepWater
};

class FilteredClosestRayResultCallback;

class CharacterController : public ThingController {
private:
	StandingMaterial GetStandingMaterial(Thing& thing);

	void PlayRunningAnimation(Id<Thing> thing_id, Thing& thing, double speed);
	void PlayStandingAnimation(Id<Thing> thing_id, Thing& thing);

	void RunFallingSweep(Id<Thing> thing_id, Thing& thing, double dt, FilteredClosestRayResultCallback& rrcb);
	void RunWalkingSweep(Id<Thing> thing_id, Thing& thing, double dt, FilteredClosestRayResultCallback& rrcb);
	void UpdateFalling(Id<Thing> thing_id, Thing& thing, double dt);
	void UpdateStandingOnSurface(Id<Thing> thing_id, Thing& thing, double dt);
	void UpdateStandingOnThing(Id<Thing> thing_id, Thing& thing, double dt);
	void StepOnSurface(Id<Thing> thing_id, Thing& thing, unsigned int surf_id, const FilteredClosestRayResultCallback& rrcb);
	void StepOnThing(Id<Thing> thing_id, Thing& thing, Id<Thing> step_thing_id, const FilteredClosestRayResultCallback& rrcb);
	void LandOnSurface(Id<Thing> thing_id, Thing& thing, unsigned int surf_id, const FilteredClosestRayResultCallback& rrcb);
	void LandOnThing(Id<Thing> thing_id, Thing& thing, Id<Thing> land_thing_id, const FilteredClosestRayResultCallback& rrcb);
	void Jump(Id<Thing> thing_id, Thing& thing);
	void JumpFromSurface(Id<Thing> thing_id, Thing& thing, unsigned int surf_id);
	void JumpFromThing(Id<Thing> thing_id, Thing& thing, Id<Thing> jump_thing_id);

public:
	using ThingController::ThingController;

	virtual void Update(Id<Thing> thing_id, double dt) override;
	virtual void RemoveControllerData(Id<Thing> thing_id) override;
	virtual void CreateControllerData(Id<Thing> thing_id) override;
	virtual void HandleAnimationMarker(Id<Thing> thing_id, Flags::KeyMarkerType marker) override;

	void PlayLeftRunFootstep(Id<Thing> thing_id);
	void PlayRightRunFootstep(Id<Thing> thing_id);
};

}
}
}
}
}
