#include "mock_keyboard.hpp"

bool mock_keyboard::is_key_down(gorc::keyboard_key) const
{
    return false;
}
