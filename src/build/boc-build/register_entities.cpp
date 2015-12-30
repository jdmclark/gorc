#include "register_entities.hpp"

#include "entities/file_entity.hpp"
#include "entities/graph_entity.hpp"
#include "entities/root_entity.hpp"

void gorc::register_boc_entities(entity_registry &reg)
{
    reg.register_entity<file_entity>();
    reg.register_entity<graph_entity>();
    reg.register_entity<root_entity>();
}
