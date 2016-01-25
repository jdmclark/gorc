#include "test/test.hpp"
#include "mock_keyboard.hpp"
#include "input/bound/keyboard_button_binding.hpp"
#include "input/bound/button_bindable_command.hpp"

using namespace gorc;

begin_suite(keyboard_button_binding_test);

test_case(pressed_set)
{
    class kbbt_mock_keyboard : public mock_keyboard {
    public:
        bool is_pressed = false;

        virtual bool is_key_down(keyboard_key k) const
        {
            assert_eq(k, keyboard_key::backspace);
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

    kbbt_mock_keyboard kb;
    mock_bindable_command bc;

    keyboard_button_binding kbb(bc, keyboard_key::backspace);

    assert_true(!bc.is_pressed);

    kb.is_pressed = false;
    kbb.handle_keyboard_input(0.0s, kb);

    assert_true(!bc.is_pressed);

    kb.is_pressed = true;
    kbb.handle_keyboard_input(0.0s, kb);

    assert_true(bc.is_pressed);
}

end_suite(keyboard_button_binding_test);
