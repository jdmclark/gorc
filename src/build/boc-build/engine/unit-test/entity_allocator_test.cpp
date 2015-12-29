#include "test/test.hpp"
#include "../entity_allocator.hpp"

using namespace gorc;

namespace {

    std::unordered_set<entity*> empty_deps;

    class mock_file_entity : public base_file_entity {
    public:
        using base_file_entity::base_file_entity;

        virtual std::unordered_set<entity*> const& dependencies() override
        {
            return empty_deps;
        }

        virtual bool is_dirty() override
        {
            return false;
        }

        virtual bool update(service_registry const &) override
        {
            return true;
        }

        virtual void serialize(entity_output_stream &) override
        {
            return;
        }
    };

    class child_mock_file_entity : public mock_file_entity {
    public:
        using mock_file_entity::mock_file_entity;
    };

}

begin_suite(entity_allocator_test);

test_case(basic_filename_entity_reuse)
{
    entity_allocator ea;

    mock_file_entity *ent1 = ea.emplace<mock_file_entity>("foo.bar");
    mock_file_entity *ent2 = ea.emplace<mock_file_entity>("foo.bar");

    assert_eq(ent1, ent2);
}

test_case(filename_down_cast)
{
    entity_allocator ea;

    child_mock_file_entity *ent1 = ea.emplace<child_mock_file_entity>("foo.bar");
    mock_file_entity *ent2 = ea.emplace<mock_file_entity>("foo.bar");

    assert_eq(ent1, ent2);
}

test_case(filename_incompatible_cast)
{
    entity_allocator ea;

    ea.emplace<mock_file_entity>("foo.bar");

    assert_throws_logged(ea.emplace<child_mock_file_entity>("foo.bar"));
    assert_log_message(log_level::error, "entity 'foo.bar' type mismatch");
    assert_log_empty();
}

end_suite(entity_allocator_test);
