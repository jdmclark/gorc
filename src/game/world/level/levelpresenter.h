#pragma once

#include "framework/place/presenter.h"
#include "game/components.h"
#include "levelplace.h"
#include "cogtimerstate.h"
#include "cogcontinuation.h"
#include "cog/vm/virtualmachine.h"

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
	Components& components;
	LevelPlace place;
	std::unique_ptr<LevelModel> model;
	sf::Sound AmbientSound;
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
			int SenderId, int SenderRef, Content::Assets::MessageType SenderType,
			int SourceRef = -1, Content::Assets::MessageType SourceType = Content::Assets::MessageType::Nothing,
			int Param0 = 0, int Param1 = 0, int Param2 = 0, int Param3 = 0);
	void SendMessageToAll(Cog::MessageId message, int SenderId, int SenderRef, Content::Assets::MessageType SenderType,
				int SourceRef = -1, Content::Assets::MessageType SourceType = Content::Assets::MessageType::Nothing,
				int Param0 = 0, int Param1 = 0, int Param2 = 0, int Param3 = 0);
	void SendMessageToLinked(Cog::MessageId message, int SenderRef, Content::Assets::MessageType SenderType,
			int SourceRef = -1, Content::Assets::MessageType SourceType = Content::Assets::MessageType::Nothing,
			int Param0 = 0, int Param1 = 0, int Param2 = 0, int Param3 = 0);

public:
	LevelPresenter(Components& components, const LevelPlace& place);

	void Start(Event::EventBus& eventBus);
	void Update(double dt);
	void UpdateSimulation(double dt);

	void TranslateCamera(const Math::Vector<3>& amt);
	void YawCamera(double amt);
	void PitchCamera(double amt);
	void Respawn();
	void Activate();

	// Anim / Cel verbs
	int SurfaceAnim(int surface, float rate, FlagSet<Content::Assets::SurfaceAnimationFlag> flags);
	int GetSurfaceAnim(int surface);

	void StopAnim(int anim);

	int GetSurfaceCel(int surface);
	void SetSurfaceCel(int surface, int cel);

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

	void SetTimer(float time);
	void Sleep(float time);

	// Player verbs
	int GetLocalPlayerThing();

	// Sector verbs
	void SetSectorAdjoins(int sector_id, bool state);
	void SetSectorLight(int sector_id, float value, float delay);

	// Sound verbs
	int PlaySoundLocal(int wav, float volume, float panning, FlagSet<Content::Assets::SoundFlag> flags);
	int PlaySoundPos(int wav, Math::Vector<3> pos, float volume, float minrad, float maxrad, FlagSet<Content::Assets::SoundFlag> flags);
	int PlaySoundThing(int wav, int thing, float volume, float minrad, float maxrad, FlagSet<Content::Assets::SoundFlag> flags);

	// Surface verbs
	Math::Vector<3> GetSurfaceCenter(int surface);

	// Thing action verbs
	int CreateThingAtThing(int tpl_id, int thing_id);
	bool IsThingMoving(int thing_id);

	// Thing property verbs
	int GetThingSector(int thing_id);
};

}
}
}
}
