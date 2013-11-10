#pragma once

#include "cog/vm/value.h"
#include "cog/vm/virtual_machine.h"
#include "cog/message_id.h"
#include <vector>

namespace gorc {
namespace cog {

class script;

class instance {
public:
	const cog::script& script;
	std::vector<vm::value> heap;

	instance(const cog::script& script);

	void call(verbs::verb_table& verbTable, vm::virtual_machine& vm, message_id message);
};

}
}
