#include "test/test.hpp"
#include "input/bound/input_binding.hpp"
#include "mock_keyboard.hpp"
#include "mock_mouse.hpp"

using namespace gorc;

begin_suite(input_binding_test);

test_case(default_calls)
{
    mock_keyboard mk;
    mock_mouse mm;

    class mock_input_binding : public input_binding {
    public:
    };

    mock_input_binding mb;

    mb.handle_mouse_input(0.0s, mm);
    mb.handle_keyboard_input(0.0s, mk);
    mb.handle_mouse_velocity(0.0s, make_zero_vector<2, float>());
}

end_suite(input_binding_test);
