#pragma once

#include "framework/place/presenter.h"
#include "game/components.h"
#include "levelplace.h"
#include "cogtimerstate.h"
#include "cogcontinuation.h"
#include "sectorbroadphasefilter.h"
#include "cog/vm/virtualmachine.h"
#include "music.h"
#include "content/flags/animflag.h"
#include "content/flags/damageflag.h"
#include "game/flags/difficultymode.h"

#include <SFML/Audio.hpp>

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

	Components& components;
	LevelPlace place;
	std::unique_ptr<LevelModel> model;
	sf::Sound AmbientSound;
	Music AmbientMusic;
	Cog::VM::VirtualMachine VirtualMachine;

	std::stack<CogContinuation> RunningCogState;

	void UpdateCameraAmbientSound();

	void UpdateThingPathMoving(unsigned int thing_id, Thing& thing, double dt);

	bool ThingInsideSector(Thing& thing, const Content::Assets::LevelSector& sec);
	bool ThingPathPassesThroughAdjoin(Thing& thing, const Math::Vector<3>& oldThingPosition,
			const Content::Assets::LevelSector& sec, const Content::Assets::LevelSurface& surf);
	bool InnerUpdateThingSector(Thing& thing, const Math::Vector<3>& oldThingPosition, const Content::Assets::LevelSector& sector,
			std::vector<std::tuple<unsigned int, unsigned int>>& path);
	void UpdateThingSector(int thing_id, Thing& thing, const Math::Vector<3>& oldThingPosition);

	void SendMessage(unsigned int InstanceId, Cog::MessageId message,
			int SenderId, int SenderRef, Flags::MessageType SenderType,
			int SourceRef = -1, Flags::MessageType SourceType = Flags::MessageType::Nothing,
			int Param0 = 0, int Param1 = 0, int Param2 = 0, int Param3 = 0);
	void SendMessageToAll(Cog::MessageId message, int SenderId, int SenderRef, Flags::MessageType SenderType,
			int SourceRef = -1, Flags::MessageType SourceType = Flags::MessageType::Nothing,
			int Param0 = 0, int Param1 = 0, int Param2 = 0, int Param3 = 0);
	void SendMessageToLinked(Cog::MessageId message, int SenderRef, Flags::MessageType SenderType,
			int SourceRef = -1, Flags::MessageType SourceType = Flags::MessageType::Nothing,
			int Param0 = 0, int Param1 = 0, int Param2 = 0, int Param3 = 0);

	void PlayFoleyLoopClass(int thing, Flags::SoundSubclassType subclass);
	void StopFoleyLoop(int thing);

public:
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

	// Anim / Cel verbs
	int SurfaceAnim(int surface, float rate, FlagSet<Flags::AnimFlag> flags);
	int GetSurfaceAnim(int surface);

	void StopAnim(int anim);

	int GetSurfaceCel(int surface);
	void SetSurfaceCel(int surface, int cel);

	int SlideCeilingSky(float u_speed, float v_speed);
	int SlideSurface(int surface_id, const Math::Vector<3>& direction);

	// Frame verbs
	int GetCurFrame(int thing_id);
	void MoveToFrame(int thing_id, int frame, float speed);

	// Message verbs
	inline int GetParam(int param_num) { return RunningCogState.top().Params[param_num % 4]; }
	inline int GetSenderId() { return RunningCogState.top().SenderId; }
	inline int GetSenderRef() {	return RunningCogState.top().SenderRef; }
	inline int GetSenderType() { return static_cast<int>(RunningCogState.top().SenderType); }
	inline int GetSourceRef() { return RunningCogState.top().SourceRef; }
	inline int GetSourceType() { return static_cast<int>(RunningCogState.top().SourceType); }

	void SetPulse(float time);
	void SetTimer(float time);
	void Sleep(float time);

	// Player verbs
	int GetLocalPlayerThing();

	// Sector verbs
	void SetSectorAdjoins(int sector_id, bool state);
	void SetSectorLight(int sector_id, float value, float delay);
	void SetSectorThrust(int sector_id, const Math::Vector<3>& thrust);
	void SetSectorTint(int sector_id, const Math::Vector<3>& color);

	// Sound verbs
	void PlaySong(int start, int end, int loopto);
	int PlaySoundClass(int thing, Flags::SoundSubclassType subclass);
	int PlaySoundLocal(int wav, float volume, float panning, FlagSet<Flags::SoundFlag> flags);
	int PlaySoundPos(int wav, Math::Vector<3> pos, float volume, float minrad, float maxrad, FlagSet<Flags::SoundFlag> flags);
	int PlaySoundThing(int wav, int thing, float volume, float minrad, float maxrad, FlagSet<Flags::SoundFlag> flags);
	void SetMusicVol(float volume);

	// Surface verbs
	void ClearAdjoinFlags(int surface, FlagSet<Flags::AdjoinFlag> flags);
	Math::Vector<3> GetSurfaceCenter(int surface);
	void SetAdjoinFlags(int surface, FlagSet<Flags::AdjoinFlag> flags);

	// Thing action verbs
	int CreateThingAtThing(int tpl_id, int thing_id);
	float DamageThing(int thing_id, float damage, FlagSet<Flags::DamageFlag> flags, int damager_id);
	Math::Vector<3> GetThingPos(int thing_id);
	bool IsThingMoving(int thing_id);

	// Thing property verbs
	int GetThingSector(int thing_id);
};

}
}
}
}
