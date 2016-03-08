#include "instance.hpp"
#include "script.hpp"

gorc::cog::instance::instance(const gorc::cog::script& script)
    : script(script) {
    return;
}

void gorc::cog::instance::call(verbs::verb_table& verbTable, vm::virtual_machine& vm, message_id message, void* system) {
    size_t jump_target = script.jump_table.get_target(message);
    vm.execute(heap, script.code, jump_target, verbTable, system);
}
