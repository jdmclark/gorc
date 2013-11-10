#pragma once

#include "type.h"
#include "cog/vm/value.h"
#include <string>

namespace gorc {
namespace cog {
namespace symbols {

class symbol {
public:
	const symbol_type type;
	const std::string name;

	const vm::value default_value;

	const bool local;
	const std::string desc;
	const int mask;
	const int link_id;
	const bool no_link;

	symbol(symbol_type type, const std::string& name, const vm::value& defaultvalue,
		bool local, const std::string& desc, int mask, int linkid, bool nolink);
};

}
}
}
