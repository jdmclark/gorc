#include "test/test.hpp"
#include "jk/content/colormap.hpp"

using namespace gorc;

begin_suite(colormap_test);

test_case(get_color_and_light_color)
{
    // Construct mock colormap
    colormap cmp(uninit_constructor);
    cmp.palette = std::make_unique<colormap_palette>(uninit_constructor);
    cmp.light_levels = std::make_unique<colormap_light_level>(uninit_constructor);

    for(int i = 0; i < 256; ++i) {
        uint8_t value = static_cast<uint8_t>(i);
        cmp.palette->data[i] = make_color_rgb8(value, value, value);
    }

    for(int i = 0; i < 64; ++i) {
        for(int j = 0; j < 256; ++j) {
            uint8_t value = static_cast<uint8_t>(i);
            cmp.light_levels->data[i][j] = value;
        }
    }

    assert_eq(cmp.get_color(5, 10), make_color_rgb8(10, 10, 10));
    assert_eq(cmp.get_color(12), make_color_rgb8(63, 63, 63));
    assert_eq(cmp.get_light_color(15), make_color_rgb8(0, 0, 0));
}

end_suite(colormap_test);
