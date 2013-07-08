#pragma once

#include "content/assets/flags.h"
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
	Content::Assets::MessageType SenderType;
	int SourceRef;
	Content::Assets::MessageType SourceType;
	std::array<int, 4> Params;

	unsigned int ProgramCounter;

	CogContinuation() = default;
	CogContinuation(unsigned int InstanceId,
			int SenderId, int SenderRef, Content::Assets::MessageType SenderType,
			int SourceRef, Content::Assets::MessageType SourceType,
			int Param0, int Param1, int Param2, int Param3,
			unsigned int ProgramCounter = 0);
};

}
}
}
}
