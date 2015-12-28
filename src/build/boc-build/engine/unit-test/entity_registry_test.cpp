#include "test/test.hpp"
#include "build/boc-build/engine/entity_registry.hpp"
#include "mock_entity.hpp"

using namespace gorc;

begin_suite(entity_registry_test);

test_case(unregistered_entity)
{
    entity_registry reg;

    assert_throws_type(reg.get_type_id<mock_entity>(), std::out_of_range);
}

test_case(first_is_zero)
{
    entity_registry reg;
    reg.register_entity<mock_entity>();

    assert_eq(reg.get_type_id<mock_entity>(), uint32_t(0));
}

end_suite(entity_registry_test);
