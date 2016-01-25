#include "test/test.hpp"
#include "input/bound/repeat_button_bindable_command.hpp"

using namespace gorc;

class repeat_button_bindable_command_test_fixture : public test::fixture {
public:
    int press_count = 0;
    repeat_button_bindable_command bc =
        repeat_button_bindable_command(1.0s,
                                       0.5s,
                                       [&](time_delta) { ++press_count; });
};

begin_suite_fixture(repeat_button_bindable_command_test,
                    repeat_button_bindable_command_test_fixture);

test_case(initial)
{
    assert_eq(0, press_count);
}

test_case(tap)
{
    assert_eq(0, press_count);

    bc.set_pressed();
    assert_eq(0, press_count);

    bc.update(0.0s);
    assert_eq(1, press_count);

    bc.update(0.0s);
    assert_eq(1, press_count);
}

test_case(hold)
{
    assert_eq(0, press_count);

    bc.set_pressed();
    bc.update(0.0s);
    assert_eq(1, press_count);

    bc.set_pressed();
    bc.update(0.5s);
    assert_eq(1, press_count);

    bc.set_pressed();
    bc.update(0.5s);
    assert_eq(2, press_count);

    bc.set_pressed();
    bc.update(0.5s);
    assert_eq(3, press_count);

    bc.set_pressed();
    bc.update(0.5s);
    assert_eq(4, press_count);
}

test_case(multiple)
{
    assert_eq(0, press_count);

    bc.set_pressed();
    bc.update(2.0s);

    assert_eq(4, press_count);
}

end_suite(repeat_button_bindable_command_test);
