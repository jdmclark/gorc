#pragma once

#include "type.h"
#include "cog/vm/value.h"
#include <string>

namespace Gorc {
namespace Cog {
namespace Symbols {

class Symbol
{
public:
	const SymbolType Type;
	const std::string Name;

	const VM::Value DefaultValue;

	const bool Local;
	const std::string Desc;
	const int Mask;
	const int Linkid;
	const bool Nolink;

	Symbol(SymbolType type, const std::string& name, const VM::Value& defaultValue,
		bool local, const std::string& desc, int mask, int linkid, bool nolink);
};

}
}
}
