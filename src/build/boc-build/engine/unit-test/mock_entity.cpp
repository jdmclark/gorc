#include "mock_entity.hpp"
#include "utility/make_unique.hpp"

mock_entity::mock_entity(std::string const &name)
    : name_value(name)
{
    return;
}

std::string const& mock_entity::name()
{
    return name_value;
}

std::unordered_set<gorc::entity*> const& mock_entity::dependencies()
{
    return dependencies_value;
}

bool mock_entity::is_dirty()
{
    return is_dirty_value;
}

bool mock_entity::update()
{
    is_dirty_value = false;
    return true;
}

namespace {
    mock_entity* get_node(std::string const &name,
                          std::unordered_map<std::string, std::unique_ptr<gorc::entity>> &rv)
    {
        auto it = rv.find(name);
        if(it == rv.end()) {
            it = rv.emplace(name, gorc::make_unique<mock_entity>(name)).first;
        }

        // Safe: mock graph only contains mock entities.
        return reinterpret_cast<mock_entity*>(it->second.get());
    }
}

std::unordered_map<std::string, std::unique_ptr<gorc::entity>> construct_mock_entity_graph(
        std::vector<std::tuple<char const *, char const *>> const &edges,
        std::unordered_set<std::string> const &dirty_nodes)
{
    std::unordered_map<std::string, std::unique_ptr<gorc::entity>> rv;

    for(auto const &edge : edges) {
        get_node(std::get<0>(edge), rv)->dependencies_value.emplace(get_node(std::get<1>(edge), rv));
    }

    for(auto const &dirty : dirty_nodes) {
        get_node(dirty, rv)->is_dirty_value = true;
    }

    return rv;
}
