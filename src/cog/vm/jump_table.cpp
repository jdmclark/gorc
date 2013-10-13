#include "jump_table.h"

Gorc::Cog::VM::JumpTable::JumpTable()
	: table(64, 0) {
	return;
}
