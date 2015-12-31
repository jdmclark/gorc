#include "list_targets.hpp"
#include <iostream>
#include <unordered_set>
#include <map>
#include <set>

void gorc::list_targets(root_entity *root)
{
    std::unordered_set<entity*> done_entities;
    std::unordered_set<entity*> pending_entities;

    std::multimap<std::string, entity*> entity_name_map;

    pending_entities.insert(root);

    while(!pending_entities.empty()) {
        auto it = pending_entities.begin();
        entity *ent = *it;
        pending_entities.erase(it);
        done_entities.insert(ent);
        entity_name_map.emplace(ent->name(), ent);

        for(entity *dep : ent->dependencies()) {
            if(done_entities.find(dep) == done_entities.end()) {
                pending_entities.insert(dep);
            }
        }
    }

    for(auto const &ent : entity_name_map) {
        std::set<std::string> dep_names;
        for(entity *dep : ent.second->dependencies()) {
            dep_names.insert(dep->name());
        }

        std::cout << ent.first << ":" << std::endl;
        for(auto const &dep_name : dep_names) {
            std::cout << "    " << dep_name << std::endl;
        }

        std::cout << std::endl;
    }
}
