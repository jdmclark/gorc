#include "test/test.hpp"
#include "mock_mouse.hpp"
#include "input/bound/mouse_button_binding.hpp"
#include "input/bound/button_bindable_command.hpp"

using namespace gorc;

begin_suite(mouse_button_binding_test);

test_case(pressed_set)
{
    class mbbt_mock_mouse : public mock_mouse {
    public:
        bool is_pressed = false;

        virtual bool is_button_down(mouse_button m) const
        {
            assert_eq(m, mouse_button::left);
            return is_pressed;
        }
    };

    class mock_bindable_command : public button_bindable_command {
    public:
        bool is_pressed = false;

        virtual void set_pressed() override
        {
            is_pressed = true;
        }

        virtual void update(time_delta) override { }
    };

    mbbt_mock_mouse mouse;
    mock_bindable_command bc;

    mouse_button_binding mbb(bc, mouse_button::left);

    assert_true(!bc.is_pressed);

    mouse.is_pressed = false;
    mbb.handle_mouse_input(0.0s, mouse);

    assert_true(!bc.is_pressed);

    mouse.is_pressed = true;
    mbb.handle_mouse_input(0.0s, mouse);

    assert_true(bc.is_pressed);
}

end_suite(mouse_button_binding_test);
