#pragma once

#include "framework/math/vector.h"
#include "framework/utility/flag_set.h"
#include "framework/text/tokenizer.h"
#include "framework/utility/flag_set.h"
#include "cog/compiler.h"
#include "content/flags/inventory_flag.h"
#include "script.h"
#include <string>
#include <memory>

namespace gorc {
namespace content {

class manager;

namespace assets {

class inventory_bin {
public:
	std::string name;
	int bin_id;
	int min_value;
	int max_value;
	flag_set<flags::inventory_flag> flags;

	script const* cog = nullptr;

	void parse_args(text::tokenizer& tok, manager& manager, const cog::compiler& compiler, diagnostics::report& report);
};

}
}
}
