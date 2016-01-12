#include "mock_entity.hpp"
#include "../entity_serializer.hpp"
#include "../entity_deserializer.hpp"
#include "log/log.hpp"

mock_entity::mock_entity(gorc::entity_input_stream &is)
    : name_value(is.read_string())
{
    uint32_t num_deps = is.read_uint32();
    for(uint32_t i = 0; i < num_deps; ++i) {
        dependencies_value.insert(is.read_entity_reference<gorc::entity>());
    }
}

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

bool mock_entity::update(gorc::service_registry const &)
{
    LOG_INFO(gorc::format("%s build %s") %
             name_value %
             (will_update_succeed ? "succeeded" : "failed"));
    is_dirty_value = false;
    return will_update_succeed;
}

void mock_entity::serialize(gorc::entity_output_stream &os)
{
    os.write_string(name_value);

    os.write_uint32(static_cast<uint32_t>(dependencies_value.size()));
    for(entity *dep : dependencies_value) {
        os.write_entity_reference(dep);
    }
}

std::type_index mock_entity::get_type_index() const
{
    return typeid(mock_entity);
}

namespace {
    mock_entity* get_node(std::string const &name,
                          std::unordered_map<std::string, std::unique_ptr<gorc::entity>> &rv)
    {
        auto it = rv.find(name);
        if(it == rv.end()) {
            it = rv.emplace(name, std::make_unique<mock_entity>(name)).first;
        }

        // Safe: mock graph only contains mock entities.
        return reinterpret_cast<mock_entity*>(it->second.get());
    }
}

std::unordered_map<std::string, std::unique_ptr<gorc::entity>> construct_mock_entity_graph(
        std::vector<std::tuple<char const *, char const *>> const &edges,
        std::unordered_set<std::string> const &dirty_nodes,
        std::unordered_set<std::string> const &fail_nodes)
{
    std::unordered_map<std::string, std::unique_ptr<gorc::entity>> rv;

    for(auto const &edge : edges) {
        get_node(std::get<0>(edge), rv)->dependencies_value.emplace(get_node(std::get<1>(edge), rv));
    }

    for(auto const &dirty : dirty_nodes) {
        get_node(dirty, rv)->is_dirty_value = true;
    }

    for(auto const &failed : fail_nodes) {
        get_node(failed, rv)->will_update_succeed = false;
    }

    return rv;
}
