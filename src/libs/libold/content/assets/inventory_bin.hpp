#pragma once

#include "math/vector.hpp"
#include "libold/base/text/tokenizer.hpp"
#include "utility/flag_set.hpp"
#include "libold/content/flags/inventory_flag.hpp"
#include "script.hpp"
#include <string>
#include <memory>

namespace gorc {
namespace content {

class content_manager;

namespace assets {

class inventory_bin {
public:
    std::string name;
    int bin_id;
    int min_value;
    int max_value;
    flag_set<flags::inventory_flag> flags;

    script const* cog = nullptr;

    void parse_args(text::tokenizer& tok, content_manager& manager);
};

}
}
}
