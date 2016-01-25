#include "test/test.hpp"
#include "input/bound/press_button_bindable_command.hpp"

using namespace gorc;

class press_button_bindable_command_test_fixture : public test::fixture {
public:
    bool has_been_pressed = false;
    press_button_bindable_command bc =
        press_button_bindable_command([&](time_delta) { has_been_pressed = true; });
};

begin_suite_fixture(press_button_bindable_command_test, press_button_bindable_command_test_fixture);

test_case(initial)
{
    assert_true(!has_been_pressed);
}

test_case(tap)
{
    assert_true(!has_been_pressed);

    bc.set_pressed();
    assert_true(!has_been_pressed);

    bc.update(0.0s);
    assert_true(has_been_pressed);
    has_been_pressed = false;

    bc.update(0.0s);
    assert_true(!has_been_pressed);
}

test_case(hold)
{
    assert_true(!has_been_pressed);

    bc.set_pressed();
    assert_true(!has_been_pressed);

    bc.update(0.0s);
    assert_true(has_been_pressed);
    has_been_pressed = false;

    bc.set_pressed();
    bc.update(0.0s);
    assert_true(!has_been_pressed);
}

end_suite(press_button_bindable_command_test);
