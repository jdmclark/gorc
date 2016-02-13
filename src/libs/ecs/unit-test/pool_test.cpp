#include "test/test.hpp"
#include "ecs/pool.hpp"
#include <set>

namespace {

    class mock_component {
    public:
        int i = 0;

        mock_component(int i)
            : i(i)
        {
            return;
        }
    };

}

begin_suite(pool_test);

test_case(simple_emplace_erase_reuse)
{
    gorc::pool<mock_component> p;

    auto *comp = &p.emplace(102);
    assert_eq(comp->i, 102);

    auto *comp2 = &p.emplace(1583);
    assert_eq(comp->i, 102);
    assert_eq(comp2->i, 1583);
    assert_ne(comp, comp2);

    p.erase(*comp);

    auto *comp3 = &p.emplace(21);
    assert_eq(comp3->i, 21);
    assert_eq(comp, comp3);

    auto *comp4 = &p.emplace(68);
    assert_ne(comp2, comp4);
    assert_ne(comp3, comp4);
}

test_case(page_allocation)
{
    std::set<mock_component*> components;

    gorc::pool<mock_component, 5> p;
    for(int i = 0; i < 100; ++i) {
        auto *comp = &p.emplace(i);
        components.insert(comp);
        assert_eq(comp->i, i);
    }

    assert_eq(components.size(), size_t(100));
}

end_suite(pool_test);
