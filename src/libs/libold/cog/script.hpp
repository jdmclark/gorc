#pragma once

#include "libold/cog/symbols/table.hpp"
#include "libold/cog/vm/jump_table.hpp"
#include "libold/cog/vm/code_buffer.hpp"
#include "libold/cog/flags/cog_flag.hpp"
#include "utility/flag_set.hpp"
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
