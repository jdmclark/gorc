#pragma once

#include "cog/symbols/table.h"
#include "cog/vm/jumptable.h"
#include "cog/vm/codebuffer.h"
#include "cog/instance.h"
#include "cog/flags.h"
#include "framework/flagset.h"
#include <memory>

namespace Gorc {
namespace Cog {

class Script {
public:
	Script();

	FlagSet<CogFlag> Flags;
	Symbols::SymbolTable SymbolTable;
	VM::JumpTable JumpTable;
	VM::CodeBuffer Code;

	std::unique_ptr<Instance> CreateInstance() const;
	std::unique_ptr<Instance> CreateInstance(const std::vector<VM::Value>& values) const;
};

}
}
