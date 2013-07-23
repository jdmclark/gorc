#include "cogcontinuation.h"

Gorc::Game::World::Level::CogContinuation::CogContinuation(unsigned int InstanceId,
		int SenderId, int SenderRef, Content::Assets::MessageType SenderType,
		int SourceRef, Content::Assets::MessageType SourceType,
		Cog::VM::Value Param0, Cog::VM::Value Param1, Cog::VM::Value Param2, Cog::VM::Value Param3,
		unsigned int ProgramCounter)
	: InstanceId(InstanceId),
	  SenderId(SenderId), SenderRef(SenderRef), SenderType(SenderType),
	  SourceRef(SourceRef), SourceType(SourceType),
	  Params({ Param0, Param1, Param2, Param3 }), ProgramCounter(ProgramCounter) {
	return;
}
