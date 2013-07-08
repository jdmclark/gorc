#include "cogcontinuation.h"

Gorc::Game::World::Level::CogContinuation::CogContinuation(unsigned int InstanceId,
		int SenderId, int SenderRef, Content::Assets::MessageType SenderType,
		int SourceRef, Content::Assets::MessageType SourceType,
		int Param0, int Param1, int Param2, int Param3,
		unsigned int ProgramCounter)
	: InstanceId(InstanceId),
	  SenderId(SenderId), SenderRef(SenderRef), SenderType(SenderType),
	  SourceRef(SourceRef), SourceType(SourceType),
	  Params({ Param0, Param1, Param2, Param3 }), ProgramCounter(ProgramCounter) {
	return;
}
