#include "instance.h"
#include "script.h"

gorc::cog::instance::instance(const gorc::cog::script& script)
	: script(script) {
	return;
}

void gorc::cog::instance::call(verbs::verb_table& verbTable, vm::virtual_machine& vm, message_id message) {
	size_t jump_target = script.jump_table.get_target(message);
	vm.execute(heap, script.code, jump_target, verbTable);
}
