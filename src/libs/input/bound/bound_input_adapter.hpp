#pragma once

#include "input/input_adapter.hpp"
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>
#include "utility/time.hpp"
#include "math/vector.hpp"
#include "axis_bindable_command.hpp"
#include "button_bindable_command.hpp"
#include "input_binding.hpp"
#include "log/log.hpp"
#include <exception>
#include <boost/format.hpp>

// Types of inputs:
// Axis - axis-like, clamped to -1,1
// Button value
// Press (normal, auto repeat, delayed auto repeat) - button-like
// Hold (down and then up) - button-like
//
// Types of bindings:
// Button to button-like
// Button to axis-like direction
// Axis to button-like with threshold
// Axis to axis-like
//
// Types of value providers:
// Keyboard (button-type)
// Mouse button (button-type)
// Mouse axis
// Joystick button
// Joystick axis

namespace gorc {

    class bound_input_adapter : public input_adapter {
    private:
        std::unordered_map<std::string, std::unique_ptr<axis_bindable_command>> axis_commands;
        std::unordered_map<std::string, std::unique_ptr<button_bindable_command>> button_commands;
        std::vector<std::unique_ptr<input_binding>> bindings;

        bool input_was_skipped = true;
        bool accept_cursor = false;

    public:
        virtual void handle_mouse_cursor(time_delta, mouse&) override;

        virtual bool wants_mouse_focus() override;
        virtual bool wants_keyboard_focus() override;

        virtual void handle_mouse_input(time_delta, mouse&) override;
        virtual void handle_keyboard_input(time_delta, keyboard&) override;
        virtual void handle_text_input(time_delta, char) override;

        void update(time_delta);

        template <typename ButtonCmdT, typename... ArgT>
        void emplace_button(std::string const &name,
                            ArgT&&... args)
        {
            button_commands.emplace(name, std::make_unique<ButtonCmdT>(std::forward<ArgT>(args)...));
        }

        template <typename AxisCmdT, typename... ArgT>
        void emplace_axis(std::string const &name,
                          ArgT&&... args)
        {
            axis_commands.emplace(name, std::make_unique<AxisCmdT>(std::forward<ArgT>(args)...));
        }

        template <typename BindingT, typename... ArgT>
        void bind_button(std::string const &name,
                         ArgT&&... args)
        {
            auto it = button_commands.find(name);
            if(it == button_commands.end()) {
                LOG_FATAL(format("cannot bind to unknown command %s") % name);
            }

            bindings.emplace_back(std::make_unique<BindingT>(*it->second,
                                                             std::forward<ArgT>(args)...));
        }

        template <typename BindingT, typename... ArgT>
        void bind_axis(std::string const &name,
                       ArgT&&... args)
        {
            auto it = axis_commands.find(name);
            if(it == axis_commands.end()) {
                LOG_FATAL(format("cannot bind to unknown command %s") % name);
            }

            bindings.emplace_back(std::make_unique<BindingT>(*it->second,
                                                             std::forward<ArgT>(args)...));
        }
    };

}
