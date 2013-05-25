#pragma once

#include "cog/symbols/table.h"
#include "cog/vm/jumptable.h"
#include "cog/vm/codebuffer.h"
#include "cog/instance.h"

namespace Gorc {
namespace Cog {

class Script {
public:
	Script();

	Symbols::SymbolTable SymbolTable;
	VM::JumpTable JumpTable;
	VM::CodeBuffer Code;

	Instance CreateInstance() const;
	Instance CreateInstance(const std::vector<VM::Value>& values) const;
};

}
}
