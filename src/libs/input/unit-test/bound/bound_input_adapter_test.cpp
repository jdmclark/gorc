#include "test/test.hpp"
#include "input/bound/bound_input_adapter.hpp"
#include "mock_mouse.hpp"
#include "mock_keyboard.hpp"

using namespace gorc;

begin_suite(bound_input_adapter_test);

test_case(always_wants_focus)
{
    bound_input_adapter bindings;

    assert_true(bindings.wants_mouse_focus());
    assert_true(bindings.wants_keyboard_focus());
}

test_case(button_update_pressed)
{
    class mock_button : public button_bindable_command {
    public:
        bool &update_called;

        mock_button(bool &update_called)
            : update_called(update_called)
        {
            return;
        }

        virtual void set_pressed() override { }
        virtual void update(time_delta) override
        {
            update_called = true;
        }
    };

    bool update_called = false;
    bound_input_adapter bindings;

    bindings.emplace_button<mock_button>("foo", update_called);

    assert_true(!update_called);

    bindings.update(0.0s);

    assert_true(update_called);
}

test_case(axis_update)
{
    class mock_axis : public axis_bindable_command {
    public:
        bool &update_called;

        mock_axis(bool &update_called)
            : update_called(update_called)
        {
            return;
        }

        virtual void add_value(float) override { }
        virtual void update(time_delta) override
        {
            update_called = true;
        }
    };

    bool update_called = false;
    bound_input_adapter bindings;

    bindings.emplace_axis<mock_axis>("foo", update_called);

    assert_true(!update_called);

    bindings.update(0.0s);

    assert_true(update_called);
}

test_case(input_handled)
{
    class mock_button : public button_bindable_command {
    public:
        virtual void set_pressed() override { };
        virtual void update(time_delta) override { };
    };

    class mock_input_binding : public input_binding {
    public:
        bool &mouse_passed;
        bool &keyboard_passed;

        mock_input_binding(button_bindable_command &, bool &mouse_passed, bool &keyboard_passed)
            : mouse_passed(mouse_passed)
            , keyboard_passed(keyboard_passed)
        {
            return;
        }

        virtual void handle_mouse_input(time_delta, mouse&) override
        {
            mouse_passed = true;
        }

        virtual void handle_keyboard_input(time_delta, keyboard&) override
        {
            keyboard_passed = true;
        }
    };

    bool mouse_passed = false;
    bool keyboard_passed = false;

    bound_input_adapter bindings;
    bindings.emplace_button<mock_button>("foo");

    bindings.bind_button<mock_input_binding>("foo", mouse_passed, keyboard_passed);

    assert_true(!mouse_passed);
    assert_true(!keyboard_passed);

    mock_mouse m;
    mock_keyboard k;

    bindings.handle_mouse_input(0.0s, m);

    assert_true(mouse_passed);
    assert_true(!keyboard_passed);

    mouse_passed = false;

    bindings.handle_keyboard_input(0.0s, k);

    assert_true(!mouse_passed);
    assert_true(keyboard_passed);
}

test_case(button_not_found)
{
    class mock_input_binding : public input_binding {
    public:
        mock_input_binding(button_bindable_command&) { }
    };

    bound_input_adapter bindings;
    assert_throws_logged(bindings.bind_button<mock_input_binding>("foo"));

    assert_log_message(gorc::log_level::error, "cannot bind to unknown command foo");
    assert_log_empty();
}

test_case(axis_not_found)
{
    class mock_input_binding : public input_binding {
    public:
        mock_input_binding(axis_bindable_command&) { }
    };

    bound_input_adapter bindings;
    assert_throws_logged(bindings.bind_axis<mock_input_binding>("foo"));

    assert_log_message(gorc::log_level::error, "cannot bind to unknown command foo");
    assert_log_empty();
}

test_case(handle_mouse_cursor)
{
    class mock_axis : public axis_bindable_command {
    public:
        virtual void add_value(float) override { };
        virtual void update(time_delta) override { };
    };

    class mock_input_binding : public input_binding {
    public:
        bool &handle_vel_called;

        mock_input_binding(axis_bindable_command&, bool &handle_vel_called)
            : handle_vel_called(handle_vel_called)
        {
            return;
        }

        virtual void handle_mouse_velocity(time_delta, vector<2, float> const &vel) override
        {
            handle_vel_called = true;
            assert_true(almost_equal(vel, make_vector(0.5f, 0.5f)));
        }
    };

    class local_mock_mouse : public mock_mouse {
    public:
        bool &set_cursor_pos;
        bool &set_cursor_vis;

        local_mock_mouse(bool &set_cursor_pos, bool &set_cursor_vis)
            : set_cursor_pos(set_cursor_pos)
            , set_cursor_vis(set_cursor_vis)
        {
            return;
        }

        virtual gorc::size<2, int> get_cursor_area() const override
        {
            return make_size(100, 100);
        }

        virtual gorc::point<2, int> get_cursor_position() const override
        {
            return make_point(100, 100);
        }

        virtual void set_cursor_position(point<2, int> const &pos) override
        {
            assert_eq(pos, make_point(50, 50));
            set_cursor_pos = true;
        }

        virtual void set_cursor_visible(bool visible) override
        {
            assert_true(!visible);
            set_cursor_vis = true;
        }
    };

    bool handle_vel_called = false;
    bool set_cursor_pos = false;
    bool set_cursor_visible = false;
    local_mock_mouse m(set_cursor_pos, set_cursor_visible);
    bound_input_adapter bindings;
    bindings.emplace_axis<mock_axis>("foo");
    bindings.bind_axis<mock_input_binding>("foo", handle_vel_called);

    // Initialization takes one cycle
    assert_true(!handle_vel_called);
    assert_true(!set_cursor_pos);
    assert_true(!set_cursor_visible);
    bindings.handle_mouse_cursor(1.0s, m);
    assert_true(!handle_vel_called);
    assert_true(set_cursor_pos);
    assert_true(set_cursor_visible);
    bindings.handle_mouse_cursor(1.0s, m);
    assert_true(handle_vel_called);

    // Querying wants focus doesn't reset
    handle_vel_called = false;
    bindings.wants_mouse_focus();
    bindings.handle_mouse_cursor(1.0s, m);
    assert_true(handle_vel_called);

    // Skipping resets init
    handle_vel_called = false;
    bindings.wants_mouse_focus();
    bindings.wants_mouse_focus();
    bindings.handle_mouse_cursor(1.0s, m);
    assert_true(!handle_vel_called);
    bindings.handle_mouse_cursor(1.0s, m);
    assert_true(handle_vel_called);
}

test_case(handle_text_input)
{
    // Note: Text input is ignored.
    bound_input_adapter bindings;
    bindings.handle_text_input(0.0s, '\0');
}

end_suite(bound_input_adapter_test);
