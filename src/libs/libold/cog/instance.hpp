#pragma once

#include "libold/cog/vm/value.hpp"
#include "libold/cog/vm/virtual_machine.hpp"
#include "libold/cog/message_id.hpp"
#include <vector>

namespace gorc {
namespace cog {

class script;

class instance {
public:
    const cog::script& script;
    std::vector<vm::value> heap;

    instance(const cog::script& script);

    void call(verbs::verb_table& verbTable, vm::virtual_machine& vm, message_id message, void* system);
};

}
}
