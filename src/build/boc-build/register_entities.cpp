#include "register_entities.hpp"

#include "entities/file_entity.hpp"
#include "entities/graph_entity.hpp"
#include "entities/library_file_entity.hpp"
#include "entities/object_file_entity.hpp"
#include "entities/program_file_entity.hpp"
#include "entities/project_entity.hpp"
#include "entities/root_entity.hpp"
#include "entities/source_file_entity.hpp"

void gorc::register_boc_entities(entity_registry &reg)
{
    reg.register_entity<file_entity>();
    reg.register_entity<graph_entity>();
    reg.register_entity<library_file_entity>();
    reg.register_entity<object_file_entity>();
    reg.register_entity<program_file_entity>();
    reg.register_entity<project_entity>();
    reg.register_entity<root_entity>();
    reg.register_entity<source_file_entity>();
}
