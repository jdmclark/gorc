#pragma once

#include "framework/math/vector.h"
#include "framework/flag_set.h"
#include "framework/text/tokenizer.h"
#include "framework/flag_set.h"
#include "cog/compiler.h"
#include "content/flags/inventory_flag.h"
#include "script.h"
#include <string>
#include <memory>

namespace Gorc {
namespace Content {

class Manager;

namespace Assets {

class InventoryBin {
public:
	std::string Name;
	int BinId;
	int MinValue;
	int MaxValue;
	FlagSet<Flags::InventoryFlag> Flags;

	Script const* Cog = nullptr;

	void ParseArgs(Text::Tokenizer& tok, Manager& manager, const Cog::Compiler& compiler, Diagnostics::Report& report);
};

}
}
}
