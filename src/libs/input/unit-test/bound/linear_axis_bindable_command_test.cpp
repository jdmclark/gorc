#include "test/test.hpp"
#include "input/bound/linear_axis_bindable_command.hpp"

using namespace gorc;

begin_suite(linear_axis_bindable_command_test);

test_case(forward_on_update)
{
    bool has_forwarded = false;
    linear_axis_bindable_command lab([&](time_delta, float amt)
        {
            assert_le(amt, 500.01f);
            assert_ge(amt, 499.99f);
            has_forwarded = true;
        });

    assert_true(!has_forwarded);
    lab.add_value(500.0f);
    assert_true(!has_forwarded);
    lab.update(0.0s);
    assert_true(has_forwarded);

    has_forwarded = false;
    lab.add_value(400.0f);
    lab.add_value(100.0f);
    lab.update(0.0s);
    assert_true(has_forwarded);
}

end_suite(linear_axis_bindable_command_test);
