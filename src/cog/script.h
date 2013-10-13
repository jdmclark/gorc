#pragma once

#include "cog/symbols/table.h"
#include "cog/vm/jump_table.h"
#include "cog/vm/code_buffer.h"
#include "cog/flags/cog_flag.h"
#include "framework/flag_set.h"
#include <memory>

namespace Gorc {
namespace Cog {

class Script {
public:
	Script();

	FlagSet<Flags::CogFlag> Flags;
	Symbols::SymbolTable SymbolTable;
	VM::JumpTable JumpTable;
	VM::CodeBuffer Code;
};

}
}
