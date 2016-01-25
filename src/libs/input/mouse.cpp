#include "mouse.hpp"
#include <unordered_map>
#include "utility/enum_hash.hpp"

using namespace gorc;

namespace {

    std::unordered_map<std::string, mouse_button> string_to_mouse_button_map {
        { "left", mouse_button::left },
        { "right", mouse_button::right },
        { "middle", mouse_button::middle },
        { "xbutton1", mouse_button::xbutton1 },
        { "xbutton2", mouse_button::xbutton2 }
    };

    std::unordered_map<mouse_button, std::string, enum_hash<mouse_button>>
        mouse_button_to_string_map {
        { mouse_button::left, "left" },
        { mouse_button::right, "right" },
        { mouse_button::middle, "middle" },
        { mouse_button::xbutton1, "xbutton1" },
        { mouse_button::xbutton2, "xbutton2" }
    };

    std::string unknown_mouse_button = "unknown";

}

gorc::mouse_button gorc::string_to_mouse_button(std::string const &s)
{
    auto it = string_to_mouse_button_map.find(s);
    if(it != string_to_mouse_button_map.end()) {
        return it->second;
    }
    return mouse_button::unknown;
}

std::string const &gorc::mouse_button_to_string(mouse_button b)
{
    auto it = mouse_button_to_string_map.find(b);
    if(it != mouse_button_to_string_map.end()) {
        return it->second;
    }
    return unknown_mouse_button;
}

gorc::mouse::~mouse()
{
    return;
}
