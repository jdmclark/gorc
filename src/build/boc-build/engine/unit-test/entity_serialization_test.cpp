#include "test/test.hpp"
#include "mock_entity.hpp"
#include "../entity_serializer.hpp"
#include "../entity_deserializer.hpp"
#include "io/memory_file.hpp"
#include <map>

using namespace gorc;

namespace {

    void dump_graph(entity *ent) {
        LOG_INFO(format("Entity %s:") % ent->name());

        std::map<std::string, entity*> deps;
        for(entity *dep : ent->dependencies()) {
            deps.emplace(dep->name(), dep);
        }

        for(auto const &dep : deps) {
            LOG_INFO(format("-> %s") % dep.second->name());
        }

        for(auto const &dep : deps) {
            dump_graph(dep.second);
        }
    }

}

begin_suite(entity_serialization_test);

test_case(simple_round_trip)
{
    entity_registry er;
    er.register_entity<mock_entity>();

    auto mock_graph = construct_mock_entity_graph(
            {
                std::make_tuple("A", "B"),
                std::make_tuple("B", "C"),
                std::make_tuple("B", "D"),
                std::make_tuple("C", "E"),
                std::make_tuple("D", "E")
            });

    memory_file mf;

    // Serialize graph
    entity_serializer es(er, mf);
    es.serialize_root(mock_graph.at("A").get());

    // Deserialize graph
    entity_allocator ea;
    entity_deserializer ed(er, ea, mf);

    mock_entity *root_duplicate = ed.deserialize_root<mock_entity>();
    dump_graph(root_duplicate);

    assert_log_message(log_level::info, "Entity A:");
    assert_log_message(log_level::info, "-> B");

    assert_log_message(log_level::info, "Entity B:");
    assert_log_message(log_level::info, "-> C");
    assert_log_message(log_level::info, "-> D");

    assert_log_message(log_level::info, "Entity C:");
    assert_log_message(log_level::info, "-> E");

    assert_log_message(log_level::info, "Entity E:");

    assert_log_message(log_level::info, "Entity D:");
    assert_log_message(log_level::info, "-> E");

    assert_log_message(log_level::info, "Entity E:");

    assert_log_empty();
}

test_case(serialize_cyclic)
{
    entity_registry er;
    er.register_entity<mock_entity>();

    auto mock_graph = construct_mock_entity_graph(
            {
                std::make_tuple("A", "B"),
                std::make_tuple("B", "C"),
                std::make_tuple("C", "A")
            });

    memory_file mf;

    entity_serializer es(er, mf);

    assert_throws_logged(es.serialize_root(mock_graph.at("A").get()));
    assert_log_message(log_level::error, "cannot write database: "
                                         "cycle detected in entity 'A'");
    assert_log_empty();
}

test_case(deserialize_empty)
{
    memory_file mf;

    entity_registry er;
    entity_allocator ea;
    entity_deserializer ed(er, ea, mf);

    assert_throws_logged(ed.deserialize_root<mock_entity>());
    assert_log_message(log_level::error, "database corrupt: no root node exists");
    assert_log_empty();
}

test_case(deserialize_partial_id)
{
    memory_file mf;
    write<uint16_t>(mf, uint16_t(5));

    entity_registry er;
    entity_allocator ea;
    entity_deserializer ed(er, ea, mf);

    assert_throws_logged(ed.deserialize_root<mock_entity>());
    assert_log_message(log_level::error, "database corrupt: could not read next entity type id");
    assert_log_empty();
}

test_case(serialize_non_dep_reference)
{
    class non_dep_entity : public gorc::entity {
    private:
        std::string const name_value;
        entity *non_dep_ent;
        std::unordered_set<entity*> empty_deps;

    public:
        non_dep_entity(gorc::entity_input_stream &) { }

        non_dep_entity(std::string const &name,
                       entity *non_dep_ent)
            : name_value(name)
            , non_dep_ent(non_dep_ent)
        {
            return;
        }

        virtual std::string const& name() override
        {
            return name_value;
        }

        virtual std::unordered_set<gorc::entity*> const& dependencies() override
        {
            return empty_deps;
        }

        virtual bool is_dirty() override
        {
            return false;
        }

        virtual bool update(gorc::service_registry const &) override
        {
            return true;
        }

        virtual void serialize(gorc::entity_output_stream &os) override
        {
            os.write_string(name_value);
            os.write_entity_reference(non_dep_ent);
        }
    };

    mock_entity other("OTHER");
    non_dep_entity self("SELF", &other);

    entity_registry er;
    er.register_entity<mock_entity>();
    er.register_entity<non_dep_entity>();

    memory_file mf;

    entity_serializer es(er, mf);
    assert_throws_logged(es.serialize_root(&self));
    assert_log_message(log_level::error, "cannot write database: "
                                         "reference to unserialized entity 'OTHER'");
    assert_log_empty();
}

test_case(deserialize_root_type_mismatch)
{
    class other_ent : public entity { };

    entity_registry er;
    er.register_entity<mock_entity>();

    auto mock_graph = construct_mock_entity_graph(
            {
                std::make_tuple("A", "B"),
            });

    memory_file mf;

    // Serialize graph
    entity_serializer es(er, mf);
    es.serialize_root(mock_graph.at("A").get());

    // Deserialize graph
    entity_allocator ea;
    entity_deserializer ed(er, ea, mf);

    assert_throws_logged(ed.deserialize_root<other_ent>());
    assert_log_message(log_level::error, "database corrupt: root node type mismatch");
    assert_log_empty();
}

test_case(deserialize_inner_type_mismatch)
{
    class other_ent : public entity {
    public:
        std::string name_val;
        std::unordered_set<entity*> deps;

        other_ent(entity_input_stream &is)
        {
            name_val = is.read_string();
            uint32_t num_deps = is.read_uint32();
            for(uint32_t i = 0; i < num_deps; ++i) {
                // Assume dependencies are all 'other ents'
                deps.insert(is.read_entity_reference<other_ent>());
            }
        }

        other_ent(std::string const &name)
            : name_val(name)
        {
            return;
        }

        virtual std::string const& name() override
        {
            return name_val;
        }

        virtual std::unordered_set<entity*> const& dependencies() override
        {
            return deps;
        }

        virtual bool is_dirty() override
        {
            return false;
        }

        virtual bool update(service_registry const &) override
        {
            return true;
        }

        virtual void serialize(entity_output_stream &os) override
        {
            os.write_entity_type_id<other_ent>();
            os.write_string(name_val);
            os.write_uint32(static_cast<uint32_t>(deps.size()));
            for(entity *dep : deps) {
                os.write_entity_reference(dep);
            }
        }
    };

    entity_registry er;
    er.register_entity<mock_entity>();
    er.register_entity<other_ent>();

    mock_entity leaf("leaf");

    other_ent inner("inner");
    inner.deps.insert(&leaf);

    mock_entity root("root");
    root.dependencies_value.insert(&inner);

    memory_file mf;

    // Serialize graph
    entity_serializer es(er, mf);
    es.serialize_root(&root);

    // Deserialize graph
    entity_allocator ea;
    entity_deserializer ed(er, ea, mf);

    assert_throws_logged(ed.deserialize_root<mock_entity>());
    assert_log_message(log_level::error, "database corrupt: "
                                         "entity 'leaf' type mismatch");
    assert_log_empty();
}

end_suite(entity_serialization_test);
