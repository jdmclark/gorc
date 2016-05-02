#include "test/test.hpp"
#include "ecs/component_pool.hpp"
#include "content/id.hpp"
#include <vector>
#include <set>

using namespace gorc;

namespace {
    class mock_component {
    public:
        int value = 0;

        mock_component(int value)
            : value(value)
        {
            return;
        }
    };
}

begin_suite(component_pool_test);

test_case(simple_emplace_find)
{
    component_pool<thing_id, mock_component> p;

    auto const &comp = p.emplace(thing_id(5), 2);
    assert_eq(comp.value, 2);

    std::set<int> value;
    for(auto const &em : p.equal_range(thing_id(5))) {
        value.insert(em.second->value);
    }

    std::set<int> expected { 2 };
    assert_range_eq(value, expected);
}

test_case(erase_single)
{
    component_pool<thing_id, mock_component> p;

    for(int i = 0; i < 10; ++i) {
        p.emplace(thing_id(3), i);
    }

    std::set<int> value1;
    for(auto const &em : p.equal_range(thing_id(3))) {
        value1.insert(em.second->value);
    }

    std::set<int> expected1 { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    assert_range_eq(value1, expected1);

    auto rng = p.equal_range(thing_id(3));
    for(component_pool<thing_id, mock_component>::const_iterator it = rng.begin();
        it != rng.end(); ) {
        if(it->second->value % 2) {
            it = p.erase(it);
        }
        else {
            ++it;
        }
    }

    std::set<int> value2;
    for(auto const &em : p.equal_range(thing_id(3))) {
        value2.insert(em.second->value);
    }

    std::set<int> expected2 { 0, 2, 4, 6, 8 };
    assert_range_eq(value2, expected2);
}

test_case(erase_range)
{
    component_pool<thing_id, mock_component> p;

    for(int i = 0; i < 10; ++i) {
        p.emplace(thing_id(3), i);
    }

    std::set<int> value1;
    for(auto const &em : p.equal_range(thing_id(3))) {
        value1.insert(em.second->value);
    }

    std::set<int> expected1 { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    assert_range_eq(value1, expected1);

    p.erase(p.equal_range(thing_id(3)));

    assert_true(p.equal_range(thing_id(3)).empty());
}

test_case(erase_const_range)
{
    component_pool<thing_id, mock_component> p;
    component_pool<thing_id, mock_component> const &p2 = p;

    for(int i = 0; i < 10; ++i) {
        p.emplace(thing_id(3), i);
    }

    std::set<int> value1;
    for(auto const &em : p2.equal_range(thing_id(3))) {
        value1.insert(em.second->value);
    }

    std::set<int> expected1 { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    assert_range_eq(value1, expected1);

    p.erase(p2.equal_range(thing_id(3)));

    assert_true(p.equal_range(thing_id(3)).empty());
}

test_case(erase_equal_range)
{
    component_pool<thing_id, mock_component> p;

    for(int i = 0; i < 10; ++i) {
        p.emplace(thing_id(3), i);
    }

    assert_true(!p.equal_range(thing_id(3)).empty());
    p.erase_equal_range(thing_id(3));
    assert_true(p.equal_range(thing_id(3)).empty());
}

end_suite(component_pool_test);
