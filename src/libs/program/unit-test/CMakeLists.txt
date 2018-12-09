add_executable(program-test
    bare_options_test.cpp
    option_constraint_test.cpp
    options_test.cpp
    subcommand_test.cpp
    )

target_link_libraries(program-test
    program
    unittest
    )
