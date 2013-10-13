#pragma once

#include "cog/vm/value.h"
#include "cog/vm/virtual_machine.h"
#include "cog/message_id.h"
#include <vector>

namespace Gorc {
namespace Cog {

class Script;

class Instance {
public:
	const Cog::Script& Script;
	std::vector<VM::Value> Heap;

	Instance(const Cog::Script& Script);

	void Call(Verbs::VerbTable& verbTable, VM::VirtualMachine& vm, MessageId message);
};

}
}
