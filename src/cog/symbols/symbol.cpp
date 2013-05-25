#include "symbol.h"

Gorc::Cog::Symbols::Symbol::Symbol(SymbolType type, const std::string& name,
	const VM::Value& defaultValue, bool local, const std::string& desc,
	int mask, int linkid, bool nolink)
	: Type(type), Name(name), DefaultValue(defaultValue), Local(local),
	Desc(desc), Mask(mask), Linkid(linkid), Nolink(nolink)
{
	return;
}
