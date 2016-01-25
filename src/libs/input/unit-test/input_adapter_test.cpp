#include "test/test.hpp"
#include "input/input_adapter.hpp"

begin_suite(input_adapter_test);

test_case(destructor_called) {
    class mock_input_adapter : public gorc::input_adapter {
    private:
        bool &destructor_called;

    public:
        mock_input_adapter(bool &destructor_called)
            : destructor_called(destructor_called)
        {
            return;
        }

        ~mock_input_adapter()
        {
            destructor_called = true;
        }

        virtual void handle_mouse_cursor(gorc::time_delta, gorc::mouse&) override
        {
            return;
        }

        virtual bool wants_mouse_focus() override
        {
            return false;
        }

        virtual bool wants_keyboard_focus() override
        {
            return false;
        }

        virtual void handle_mouse_input(gorc::time_delta, gorc::mouse&) override
        {
            return;
        }

        virtual void handle_keyboard_input(gorc::time_delta, gorc::keyboard&) override
        {
            return;
        }

        virtual void handle_text_input(gorc::time_delta, char) override
        {
            return;
        }
    };

    bool destructor_called = false;

    gorc::input_adapter *input_adapter = new mock_input_adapter(destructor_called);

    expect_true(!destructor_called);

    delete input_adapter;

    assert_true(destructor_called);
}

end_suite(input_adapter_test);
