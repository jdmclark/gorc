#pragma once

#include "entity.hpp"
#include <unordered_set>

namespace gorc {

    std::unordered_set<entity*> compute_target_closure(std::unordered_set<entity*> const &);

}
