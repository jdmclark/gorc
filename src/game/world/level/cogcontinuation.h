#pragma once

#include "cog/vm/value.h"
#include "game/flags/messagetype.h"
#include <array>

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class CogContinuation {
public:
	unsigned int InstanceId;

	int SenderId;
	int SenderRef;
	Flags::MessageType SenderType;
	int SourceRef;
	Flags::MessageType SourceType;
	std::array<Cog::VM::Value, 4> Params;

	unsigned int ProgramCounter;

	CogContinuation() = default;
	CogContinuation(unsigned int InstanceId,
			int SenderId, int SenderRef, Flags::MessageType SenderType,
			int SourceRef, Flags::MessageType SourceType,
			Cog::VM::Value Param0, Cog::VM::Value Param1, Cog::VM::Value Param2, Cog::VM::Value Param3,
			unsigned int ProgramCounter = 0);
};

}
}
}
}
