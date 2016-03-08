#pragma once

#include <unordered_map>
#include <string>
#include <boost/filesystem/path.hpp>

#include "libold/cog/script.hpp"
#include "libold/cog/verbs/table.hpp"
#include "libold/cog/vm/value.hpp"
#include "libold/cog/message_id.hpp"
#include "io/input_stream.hpp"

namespace gorc {
namespace cog {

class compiler {
private:
    void add_message_id(const std::string& name, message_id value);
    void populate_tables();

    std::unordered_map<std::string, vm::value> ConstantTable;
    std::unordered_map<std::string, message_id> MessageTable;
    verbs::verb_table& verb_table;

public:
    compiler(verbs::verb_table& verbTable);

    void compile(input_stream &file, script &output) const;
};

}
}
