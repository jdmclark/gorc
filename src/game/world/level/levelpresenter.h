#pragma once

#include "framework/place/presenter.h"
#include "game/components.h"
#include "levelplace.h"
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

class LevelCogState {
public:
	int SenderId;
	int SenderRef;
	Content::Assets::MessageType SenderType;
	int SourceRef;
	Content::Assets::MessageType SourceType;
	std::array<int, 4> Params;

	LevelCogState(int SenderId, int SenderRef, Content::Assets::MessageType SenderType,
			int SourceRef, Content::Assets::MessageType SourceType,
			int Param0, int Param1, int Param2, int Param3);
};

class LevelPresenter : public Gorc::Place::Presenter {
private:
	Components& components;
	LevelPlace place;
	std::unique_ptr<LevelModel> model;
	sf::Sound AmbientSound;
	Cog::VM::VirtualMachine VirtualMachine;

	std::stack<LevelCogState> RunningCogState;

	void UpdateCameraAmbientSound();

	void SetThingCurrentSector(Thing& thing, unsigned int sector_id);
	bool ThingInsideSector(Thing& thing, const Content::Assets::LevelSector& sec);
	bool ThingPathPassesThroughAdjoin(Thing& thing, const Math::Vector<3>& oldThingPosition,
			const Content::Assets::LevelSector& sec, const Content::Assets::LevelSurface& surf);
	void UpdateThingSector(Thing& thing, const Math::Vector<3>& oldThingPosition);

	void SendMessage(Cog::Instance& script, Cog::MessageId message, int SenderId, int SenderRef, Content::Assets::MessageType SenderType,
			int SourceRef = -1, Content::Assets::MessageType SourceType = Content::Assets::MessageType::Nothing,
			int Param0 = 0, int Param1 = 0, int Param2 = 0, int Param3 = 0);
	void SendMessageToAll(Cog::MessageId message, int SenderId, int SenderRef, Content::Assets::MessageType SenderType,
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

	// Anim / Cel verbs
	int SurfaceAnim(int surface, float rate, FlagSet<Content::Assets::SurfaceAnimationFlag> flags);
	int GetSurfaceAnim(int surface);

	void StopAnim(int anim);

	int GetSurfaceCel(int surface);
	void SetSurfaceCel(int surface, int cel);

	// Message verbs
	inline int GetParam(int param_num) {
		return RunningCogState.top().Params[param_num % 4];
	}

	inline int GetSenderId() {
		return RunningCogState.top().SenderId;
	}

	inline int GetSenderRef() {
		return RunningCogState.top().SenderRef;
	}

	inline int GetSenderType() {
		return static_cast<int>(RunningCogState.top().SenderType);
	}

	inline int GetSourceRef() {
		return RunningCogState.top().SourceRef;
	}

	inline int GetSourceType() {
		return static_cast<int>(RunningCogState.top().SourceType);
	}

	// Surface verbs
	Math::Vector<3> GetSurfaceCenter(int surface);

	// Thing verbs
	int CreateThingAtThing(const char* tpl_name, int thing_id);
};

}
}
}
}
