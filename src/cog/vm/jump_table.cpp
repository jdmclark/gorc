#include "jump_table.h"

gorc::cog::vm::jump_table::jump_table()
	: table(64, 0) {
	return;
}
