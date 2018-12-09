add_executable(system-test
    env_test.cpp
    pipe_test.cpp
    self_test.cpp
    )

target_link_libraries(system-test
    system
    unittest
    )
