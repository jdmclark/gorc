#include "test/test.hpp"
#include "input/mouse.hpp"

begin_suite(mouse_test);

test_case(known_string_to_key)
{
    assert_eq(gorc::string_to_mouse_button("middle"),
              gorc::mouse_button::middle);
}

test_case(unknown_string_to_key)
{
    assert_eq(gorc::string_to_mouse_button("upper"),
              gorc::mouse_button::unknown);
}

test_case(known_key_to_string)
{
    assert_eq(gorc::mouse_button_to_string(gorc::mouse_button::xbutton1),
              std::string("xbutton1"));
}

test_case(unknown_key_to_string)
{
    assert_eq(gorc::mouse_button_to_string(static_cast<gorc::mouse_button>(95874)),
              std::string("unknown"));
}

test_case(destructor_called) {
    class mock_mouse : public gorc::mouse {
    private:
        bool &destructor_called;

    public:
        mock_mouse(bool &destructor_called)
            : destructor_called(destructor_called)
        {
            return;
        }

        ~mock_mouse()
        {
            destructor_called = true;
        }

        virtual bool is_button_down(gorc::mouse_button) const override
        {
            return false;
        }

        virtual gorc::point<2, int> get_cursor_position() const override
        {
            return gorc::make_origin_point<2, int>();
        }

        virtual bool get_cursor_visible() const override
        {
            return true;
        }

        virtual void set_cursor_position(gorc::point<2, int> const &) override
        {
            return;
        }

        virtual void set_cursor_visible(bool) override
        {
            return;
        }

        virtual gorc::size<2, int> get_cursor_area() const override
        {
            return gorc::make_size(0, 0);
        }
    };

    bool destructor_called = false;
    gorc::mouse *m = new mock_mouse(destructor_called);

    expect_true(!destructor_called);

    delete m;

    assert_true(destructor_called);
}

end_suite(mouse_test);
