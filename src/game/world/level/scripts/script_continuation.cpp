#include "script_continuation.h"

Gorc::Game::World::Level::Scripts::ScriptContinuation::ScriptContinuation(int InstanceId,
		int SenderId, int SenderRef, Flags::MessageType SenderType,
		int SourceRef, Flags::MessageType SourceType,
		Cog::VM::Value Param0, Cog::VM::Value Param1, Cog::VM::Value Param2, Cog::VM::Value Param3,
		unsigned int ProgramCounter)
	: InstanceId(InstanceId),
	  SenderId(SenderId), SenderRef(SenderRef), SenderType(SenderType),
	  SourceRef(SourceRef), SourceType(SourceType),
	  Params({ Param0, Param1, Param2, Param3 }), ProgramCounter(ProgramCounter) {
	return;
}
