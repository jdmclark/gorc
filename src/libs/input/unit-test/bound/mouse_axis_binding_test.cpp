#include "test/test.hpp"
#include "input/bound/mouse_axis_binding.hpp"
#include <vector>

using namespace gorc;

begin_suite(mouse_axis_binding_test);

test_case(command_passes_through)
{
    class mock_axis : public axis_bindable_command {
    public:
        std::vector<float> &values;

        mock_axis(std::vector<float> &values)
            : values(values)
        {
            return;
        }

        virtual void update(time_delta) override { }
        virtual void add_value(float v) override
        {
            values.push_back(v);
        }
    };

    std::vector<float> values;
    mock_axis mock_axis_inst(values);

    mouse_axis_binding mab(mock_axis_inst, make_vector(1.0f, 0.0f));
    mab.handle_mouse_velocity(0.0s, make_vector(1.0f, 0.0f));
    mab.handle_mouse_velocity(0.0s, make_vector(-1.0f, 0.0f));
    mab.handle_mouse_velocity(0.0s, make_vector(0.0f, 1.0f));
    mab.handle_mouse_velocity(0.0s, make_vector(0.0f, -1.0f));

    mouse_axis_binding mab2(mock_axis_inst, make_vector(0.0f, 1.0f));
    mab2.handle_mouse_velocity(0.0s, make_vector(1.0f, 0.0f));
    mab2.handle_mouse_velocity(0.0s, make_vector(-1.0f, 0.0f));
    mab2.handle_mouse_velocity(0.0s, make_vector(0.0f, 1.0f));
    mab2.handle_mouse_velocity(0.0s, make_vector(0.0f, -1.0f));

    assert_eq(values, std::vector<float>({ 1.0f,
                                          -1.0f,
                                           0.0f,
                                           0.0f,
                                           0.0f,
                                           0.0f,
                                           1.0f,
                                          -1.0f }));
}

end_suite(mouse_axis_binding_test);
