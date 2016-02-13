#include "test/test.hpp"
#include "ecs/sequential_entity_generator.hpp"

using namespace gorc;

begin_suite(sequential_entity_generator_test);

test_case(characteristic)
{
    sequential_entity_generator<thing_id> eg;
    for(int i = 0; i < 10; ++i) {
        assert_eq(static_cast<int>(eg.emplace()), i);
    }

    eg.erase(thing_id(5));

    assert_eq(static_cast<int>(eg.emplace()), 10);
}

end_suite(sequential_entity_generator_test);
