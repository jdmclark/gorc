#include "test/test.hpp"
#include "input/keyboard.hpp"

begin_suite(keyboard_test);

test_case(known_string_to_key)
{
    assert_eq(gorc::string_to_keyboard_key("f12"),
              gorc::keyboard_key::f12);
}

test_case(unknown_string_to_key)
{
    assert_eq(gorc::string_to_keyboard_key("akld"),
              gorc::keyboard_key::unknown);
}

test_case(known_key_to_string)
{
    assert_eq(gorc::keyboard_key_to_string(gorc::keyboard_key::f12),
              std::string("f12"));
}

test_case(unknown_key_to_string)
{
    assert_eq(gorc::keyboard_key_to_string(static_cast<gorc::keyboard_key>(95874)),
              std::string("unknown"));
}

test_case(destructor_called) {
    class mock_keyboard : public gorc::keyboard {
    private:
        bool &destructor_called;
    public:
        mock_keyboard(bool &destructor_called)
            : destructor_called(destructor_called)
        {
            return;
        }

        ~mock_keyboard()
        {
            destructor_called = true;
        }

        virtual bool is_key_down(gorc::keyboard_key) const override
        {
            return false;
        }
    };

    bool destructor_called = false;
    gorc::keyboard *k = new mock_keyboard(destructor_called);

    expect_true(!destructor_called);

    delete k;

    assert_true(destructor_called);
}

end_suite(keyboard_test);
