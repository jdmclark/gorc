#include "test/test.hpp"
#include "ecs/component_relational_mapping.hpp"
#include "ecs/component_registry.hpp"
#include <set>

using namespace gorc;

namespace {

    class mock_component {
    public:
        uid(10);
        int value;

        mock_component(int value)
            : value(value)
        {
            return;
        }
    };

    class mock_other_component {
    public:
        uid(20);
        int value;

        mock_other_component(int value)
            : value(value)
        {
            return;
        }
    };

    template <typename RangeT>
    std::set<int> mock_comp_to_range(RangeT const &rng)
    {
        std::set<int> rv;
        for(auto const &em : rng) {
            rv.insert(em.second->value);
        }

        return rv;
    }

    class component_relational_mapping_fixture : public test::fixture {
    public:
        component_registry<thing_id> cr;

        component_relational_mapping_fixture()
        {
            cr.register_component_type<mock_component>();
            cr.register_component_type<mock_other_component>();
        }
    };

}

begin_suite_fixture(component_relational_mapping_test,
                    component_relational_mapping_fixture);

test_case(emplace_find)
{
    component_relational_mapping<thing_id> crm;
    cr.register_component_types(crm);

    crm.emplace<mock_component>(thing_id(2), 5);
    crm.emplace<mock_component>(thing_id(2), 12);
    crm.emplace<mock_component>(thing_id(3), 2);

    assert_range_eq(mock_comp_to_range(crm.equal_range<mock_component>(thing_id(2))),
                    std::set<int>({ 5, 12 }));
}

test_case(all_range)
{
    component_relational_mapping<thing_id> crm;
    cr.register_component_types(crm);

    crm.emplace<mock_component>(thing_id(2), 5);
    crm.emplace<mock_component>(thing_id(3), 12);
    crm.emplace<mock_component>(thing_id(2), 2);

    std::set<std::tuple<thing_id, int>> values;
    for(auto const &em : crm.range<mock_component>()) {
        values.emplace(em.first, em.second->value);
    }

    std::set<std::tuple<thing_id, int>> expected {
            std::make_tuple(thing_id(2), 5),
            std::make_tuple(thing_id(3), 12),
            std::make_tuple(thing_id(2), 2)
        };

    assert_range_eq(values, expected);
}

test_case(erase_one)
{
    component_relational_mapping<thing_id> crm;
    cr.register_component_types(crm);

    for(int i = 0; i < 10; ++i) {
        crm.emplace<mock_component>(thing_id(1), i);
    }

    auto rng = crm.equal_range<mock_component>(thing_id(1));
    for(component_pool<thing_id, mock_component>::const_iterator it = rng.begin();
        it != rng.end();) {
        if(it->second->value % 2) {
            it = crm.erase(it);
        }
        else {
            ++it;
        }
    }

    crm.flush_erase_queue();

    assert_range_eq(mock_comp_to_range(crm.equal_range<mock_component>(thing_id(1))),
                    std::set<int>({ 0, 2, 4, 6, 8 }));
}

test_case(erase_all)
{
    component_relational_mapping<thing_id> crm;
    cr.register_component_types(crm);

    for(int i = 0; i < 10; ++i) {
        crm.emplace<mock_component>(thing_id(1), i);
    }

    auto rng = crm.equal_range<mock_component>(thing_id(1));
    crm.erase(rng.begin(), rng.end());

    crm.flush_erase_queue();

    assert_true(crm.equal_range<mock_component>(thing_id(1)).empty());
}

test_case(erase_equal_range)
{
    component_relational_mapping<thing_id> crm;
    cr.register_component_types(crm);

    for(int i = 0; i < 10; ++i) {
        crm.emplace<mock_component>(thing_id(1), i);
        crm.emplace<mock_other_component>(thing_id(1), i);
    }

    assert_true(!crm.equal_range<mock_component>(thing_id(1)).empty());
    assert_true(!crm.equal_range<mock_other_component>(thing_id(1)).empty());

    crm.erase_equal_range(thing_id(1));
    crm.flush_erase_queue();

    assert_true(crm.equal_range<mock_component>(thing_id(1)).empty());
    assert_true(crm.equal_range<mock_other_component>(thing_id(1)).empty());
}

test_case(register_duplicate)
{
    component_relational_mapping<thing_id> crm;
    crm.register_component_type<mock_component>();

    assert_log_empty();

    assert_throws_logged(crm.register_component_type<mock_component>());
    assert_log_message(log_level::error,
                       "component type with uid 10 is already registered");
    assert_log_empty();
}

test_case(not_registered)
{
    component_relational_mapping<thing_id> crm;

    assert_log_empty();

    assert_throws_logged(crm.range<mock_component>());
    assert_log_message(log_level::error,
                       "component type with uid 10 is not registered");
    assert_log_empty();
}

end_suite(component_relational_mapping_test);
