#pragma once

#include "input/keyboard.hpp"

class mock_keyboard : public gorc::keyboard {
public:
    virtual bool is_key_down(gorc::keyboard_key) const override;
};
