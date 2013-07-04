#include "instance.h"
#include "script.h"

Gorc::Cog::Instance::Instance(const Gorc::Cog::Script& Script)
	: Script(Script) {
	return;
}

void Gorc::Cog::Instance::Call(Verbs::VerbTable& verbTable, VM::VirtualMachine& vm, MessageId message) {
	size_t jump_target = Script.JumpTable.GetTarget(message);
	vm.Execute(Heap, Script.Code, jump_target, verbTable);
}
