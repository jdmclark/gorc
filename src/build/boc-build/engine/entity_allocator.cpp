#include "entity_allocator.hpp"

void gorc::entity_allocator::clear()
{
    std::lock_guard<std::mutex> lg(atomic_allocate_lock);

    entities.clear();
    file_map.clear();
}
