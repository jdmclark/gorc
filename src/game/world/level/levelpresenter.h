#pragma once

#include "framework/place/presenter.h"
#include "game/components.h"
#include "levelplace.h"
#include "cog/vm/virtualmachine.h"
#include "sounds/music.h"
#include "content/flags/animflag.h"
#include "content/flags/damageflag.h"
#include "game/flags/difficultymode.h"
#include "game/world/level/animations/animationpresenter.h"
#include "game/world/level/scripts/scriptpresenter.h"
#include "game/world/level/sounds/soundpresenter.h"
#include "game/world/level/keys/keypresenter.h"
#include "game/world/level/gameplay/actorcontroller.h"
#include "game/world/level/gameplay/playercontroller.h"
#include "game/world/level/gameplay/cogcontroller.h"
#include "game/world/level/gameplay/ghostcontroller.h"
#include "game/world/level/gameplay/itemcontroller.h"

#include <memory>
#include <stack>

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class LevelModel;
class Thing;

class LevelPresenter : public Gorc::Place::Presenter {
private:
	// In-data constants seem to be roughly 8 times greater than they should be.
	// Use RateFactor to scale all rates from input data.
	static constexpr double RateFactor = 1.0 / 8.0;

	// Scratch space
	std::vector<std::tuple<unsigned int, unsigned int>> UpdatePathSectorScratch;

	Components& components;
	LevelPlace place;

	void InitializeWorld();
	void PhysicsTickUpdate(double dt);

	bool PointInsideSector(const Math::Vector<3>& position, const Content::Assets::LevelSector& sec);
	bool PointPathPassesThroughAdjoin(const Math::Vector<3>& p0, const Math::Vector<3>& p1,
			const Content::Assets::LevelSector& sec, const Content::Assets::LevelSurface& surf);
	bool UpdatePathSector(const Math::Vector<3>& p0, const Math::Vector<3>& p1,
			const Content::Assets::LevelSector& sector, std::vector<std::tuple<unsigned int, unsigned int>>& path);

	void UpdateThingSector(int thing_id, Thing& thing, const Math::Vector<3>& oldThingPosition);
	void UpdateCamera();

public:
	std::unique_ptr<LevelModel> Model;

	Animations::AnimationPresenter AnimationPresenter;
	Scripts::ScriptPresenter ScriptPresenter;
	Sounds::SoundPresenter SoundPresenter;
	Keys::KeyPresenter KeyPresenter;

	Gameplay::ActorController ActorController;
	Gameplay::PlayerController PlayerController;
	Gameplay::CogController CogController;
	Gameplay::GhostController GhostController;
	Gameplay::ItemController ItemController;

	LevelPresenter(Components& components, const LevelPlace& place);

	void Start(Event::EventBus& eventBus);
	void Update(double dt);

	void TranslateCamera(const Math::Vector<3>& amt);
	void YawCamera(double amt);
	void PitchCamera(double amt);
	void Respawn();
	void Jump();
	void Activate();
	void Damage();
	void ThingSighted(int thing_id);

	// Frame verbs
	int GetCurFrame(int thing_id);
	void MoveToFrame(int thing_id, int frame, float speed);

	// Player verbs
	int GetLocalPlayerThing();

	// Sector verbs
	void SetSectorAdjoins(int sector_id, bool state);
	void SetSectorLight(int sector_id, float value, float delay);
	void SetSectorThrust(int sector_id, const Math::Vector<3>& thrust);
	void SetSectorTint(int sector_id, const Math::Vector<3>& color);

	// Surface verbs
	void ClearAdjoinFlags(int surface, FlagSet<Flags::AdjoinFlag> flags);
	Math::Vector<3> GetSurfaceCenter(int surface);
	void SetAdjoinFlags(int surface, FlagSet<Flags::AdjoinFlag> flags);
	void SetFaceGeoMode(int surface, Flags::GeometryMode mode);

	// System verbs
	int LoadSound(const char* sound);

	// Thing action verbs
	unsigned int CreateThing(const Content::Assets::Template& tpl, unsigned int sector_num, const Math::Vector<3>& pos, const Math::Vector<3>& orientation);
	unsigned int CreateThing(const std::string& tpl_name, unsigned int sector_num, const Math::Vector<3>& pos, const Math::Vector<3>& orientation);
	unsigned int CreateThing(int tpl_id, unsigned int sector_num, const Math::Vector<3>& pos, const Math::Vector<3>& orientation);

	int CreateThingAtThing(int tpl_id, int thing_id);
	float DamageThing(int thing_id, float damage, FlagSet<Flags::DamageFlag> flags, int damager_id);
	void DestroyThing(int thing_id);
	Math::Vector<3> GetThingPos(int thing_id);
	bool IsThingMoving(int thing_id);

	// Thing property verbs
	int GetThingSector(int thing_id);

	static void RegisterVerbs(Cog::Verbs::VerbTable&, Components&);
};

}
}
}
}
