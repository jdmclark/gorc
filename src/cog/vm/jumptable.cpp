#include "jumptable.h"

Gorc::Cog::VM::JumpTable::JumpTable()
	: table(64, 0) {
	return;
}
