#pragma once

#include "cog/symbols/table.h"
#include "cog/vm/jump_table.h"
#include "cog/vm/code_buffer.h"
#include "cog/flags/cog_flag.h"
#include "framework/utility/flag_set.h"
#include <memory>

namespace gorc {
namespace cog {

class script {
public:
    script();

    flag_set<flags::cog_flag> flags;
    symbols::symbol_table symbol_table;
    vm::jump_table jump_table;
    vm::code_buffer code;
};

}
}
