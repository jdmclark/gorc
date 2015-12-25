#pragma once

#include "entity.hpp"
#include <unordered_set>

namespace gorc {

    using entity_closure = std::unordered_set<entity*>;

    entity_closure compute_target_closure(std::unordered_set<entity*> const &);

}
