#pragma once

#include "game/flags/messagetype.h"
#include "scriptmodel.h"
#include "cog/vm/virtualmachine.h"

namespace Gorc {
namespace Content {
class Manager;
}

namespace Cog {
class Script;
class Compiler;

namespace Verbs {
class VerbTable;
}

namespace VM {
class VirtualMachine;
}
}

namespace Game {
class Components;

namespace World {
namespace Level {
class LevelModel;

namespace Scripts {

class ScriptModel;

class ScriptPresenter {
private:
	Components& components;
	LevelModel* levelModel;
	ScriptModel* model;
	Cog::VM::VirtualMachine VirtualMachine;

public:
	ScriptPresenter(Components& components);

	void Start(LevelModel& levelModel, ScriptModel& scriptModel);
	void Update(double dt);

	void CreateLevelDummyInstance();
	void CreateLevelCogInstance(const Cog::Script& script, Content::Manager& manager, Cog::Compiler& compiler,
			const std::vector<Cog::VM::Value>& values);

	void SendMessage(unsigned int InstanceId, Cog::MessageId message,
			int SenderId, int SenderRef, Flags::MessageType SenderType,
			int SourceRef = -1, Flags::MessageType SourceType = Flags::MessageType::Nothing,
			Cog::VM::Value Param0 = 0, Cog::VM::Value Param1 = 0, Cog::VM::Value Param2 = 0, Cog::VM::Value Param3 = 0);
	void SendMessageToAll(Cog::MessageId message, int SenderId, int SenderRef, Flags::MessageType SenderType,
			int SourceRef = -1, Flags::MessageType SourceType = Flags::MessageType::Nothing,
			Cog::VM::Value Param0 = 0, Cog::VM::Value Param1 = 0, Cog::VM::Value Param2 = 0, Cog::VM::Value Param3 = 0);
	void SendMessageToLinked(Cog::MessageId message, int SenderRef, Flags::MessageType SenderType,
			int SourceRef = -1, Flags::MessageType SourceType = Flags::MessageType::Nothing,
			Cog::VM::Value Param0 = 0, Cog::VM::Value Param1 = 0, Cog::VM::Value Param2 = 0, Cog::VM::Value Param3 = 0);

	// COG run-time verbs
	inline int GetParam(int param_num) { return model->RunningCogState.top().Params[param_num % 4]; }
	inline int GetSenderId() { return model->RunningCogState.top().SenderId; }
	inline int GetSenderRef() {	return model->RunningCogState.top().SenderRef; }
	inline int GetSenderType() { return static_cast<int>(model->RunningCogState.top().SenderType); }
	inline int GetSourceRef() { return model->RunningCogState.top().SourceRef; }
	inline int GetSourceType() { return static_cast<int>(model->RunningCogState.top().SourceType); }

	void SetPulse(float time);
	void SetTimer(float time);
	void SetTimerEx(float delay, int id, Cog::VM::Value param0, Cog::VM::Value param1);
	void Sleep(float time);

	static void RegisterVerbs(Cog::Verbs::VerbTable&, Components&);
};

}
}
}
}
}
