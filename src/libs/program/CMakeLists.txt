add_library(program STATIC
    abstract_argument_queue.cpp
    abstract_bare_option.cpp
    abstract_option.cpp
    at_least_one_input.cpp
    dependent_option.cpp
    mutual_exclusion.cpp
    option_constraint.cpp
    options.cpp
    required_option.cpp
    program.cpp
    subcommand.cpp
    subcommand_option.cpp
    subcommand_registry.cpp
    switch_option.cpp
    )

target_link_libraries(program
    log
    system
    utility
    )

add_subdirectory(unit-test)
add_subdirectory(tests/example-program)
