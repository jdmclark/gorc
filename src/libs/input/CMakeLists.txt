add_library(input STATIC
    bound/axis_bindable_command.cpp
    bound/bound_input_adapter.cpp
    bound/button_bindable_command.cpp
    bound/input_bindable_command.cpp
    bound/input_binding.cpp
    bound/keyboard_button_binding.cpp
    bound/linear_axis_bindable_command.cpp
    bound/mouse_axis_binding.cpp
    bound/mouse_button_binding.cpp
    bound/press_button_bindable_command.cpp
    bound/repeat_button_bindable_command.cpp
    input_adapter.cpp
    keyboard.cpp
    mouse.cpp
    )

target_link_libraries(input
    log
    math
    utility
    )

add_subdirectory(unit-test)
