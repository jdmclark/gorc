#pragma once

#include "framework/place/presenter.h"
#include "game/components.h"
#include "levelplace.h"
#include "cog/vm/virtualmachine.h"
#include "sounds/music.h"
#include "content/flags/animflag.h"
#include "content/flags/damageflag.h"
#include "content/assets/inventory.h"
#include "game/flags/difficultymode.h"
#include "game/world/level/animations/animationpresenter.h"
#include "game/world/level/scripts/scriptpresenter.h"
#include "game/world/level/sounds/soundpresenter.h"
#include "game/world/level/keys/keypresenter.h"
#include "game/world/level/gameplay/inventorypresenter.h"
#include "game/world/level/gameplay/actorcontroller.h"
#include "game/world/level/gameplay/playercontroller.h"
#include "game/world/level/gameplay/cogcontroller.h"
#include "game/world/level/gameplay/ghostcontroller.h"
#include "game/world/level/gameplay/itemcontroller.h"
#include "game/world/level/gameplay/corpsecontroller.h"

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

	Gameplay::ThingController& GetThingController(Flags::ThingType type);

	bool need_respawn = false;
	void DoRespawn();

public:
	std::unique_ptr<LevelModel> Model;

	Animations::AnimationPresenter AnimationPresenter;
	Scripts::ScriptPresenter ScriptPresenter;
	Sounds::SoundPresenter SoundPresenter;
	Keys::KeyPresenter KeyPresenter;
	Gameplay::InventoryPresenter InventoryPresenter;

	Gameplay::ActorController ActorController;
	Gameplay::PlayerController PlayerController;
	Gameplay::CogController CogController;
	Gameplay::GhostController GhostController;
	Gameplay::ItemController ItemController;
	Gameplay::CorpseController CorpseController;

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

	// Color verbs
	void AddDynamicTint(int player_id, const Math::Vector<3>& tint);

	// Frame verbs
	int GetCurFrame(int thing_id);
	void JumpToFrame(int thing_id, int frame, int sector);
	void MoveToFrame(int thing_id, int frame, float speed);

	// Level verbs
	float GetGameTime();
	float GetLevelTime();

	// Misc verbs
	void TakeItem(int thing_id, int player_id);

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
	void SetSurfaceFlags(int surface, FlagSet<Flags::SurfaceFlag> flags);

	// System verbs
	int LoadSound(const char* sound);

	// Thing action verbs
	int CreateThing(const Content::Assets::Template& tpl, unsigned int sector_num, const Math::Vector<3>& pos, const Math::Vector<3>& orientation);
	int CreateThing(const std::string& tpl_name, unsigned int sector_num, const Math::Vector<3>& pos, const Math::Vector<3>& orientation);
	int CreateThing(int tpl_id, unsigned int sector_num, const Math::Vector<3>& pos, const Math::Vector<3>& orientation);

	void AttachThingToThing(int thing_id, int base_id);
	int CreateThingAtThing(int tpl_id, int thing_id);
	float DamageThing(int thing_id, float damage, FlagSet<Flags::DamageFlag> flags, int damager_id);
	void DestroyThing(int thing_id);
	void DetachThing(int thing_id);
	Math::Vector<3> GetThingPos(int thing_id);
	bool IsThingMoving(int thing_id);

	void AdjustThingPos(int thing_id, const Math::Vector<3>& new_pos);
	void SetThingPos(int thing_id, const Math::Vector<3>& new_pos, const Math::Vector<3>& new_orient, int new_sector);

	// Thing flag verbs
	void ClearActorFlags(int thing_id, FlagSet<Flags::ActorFlag> flags);
	void ClearThingFlags(int thing_id, FlagSet<Flags::ThingFlag> flags);

	FlagSet<Flags::ActorFlag> GetActorFlags(int thing_id);

	void SetActorFlags(int thing_id, FlagSet<Flags::ActorFlag> flags);

	// Thing property verbs
	int GetThingParent(int thing_id);
	int GetThingSector(int thing_id);
	Flags::ThingType GetThingType(int thing_id);
	void SetThingType(int thing_id, Flags::ThingType type);
	void SetThingLight(int thing_id, float light, float fade_time);

	static void RegisterVerbs(Cog::Verbs::VerbTable&, Components&);
};

}
}
}
}
