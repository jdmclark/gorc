#pragma once

#include "cog/vm/value.h"
#include "cog/vm/virtualmachine.h"
#include "cog/messageid.h"
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
