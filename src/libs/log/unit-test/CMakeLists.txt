add_executable(log-test
    diagnostic_context_test.cpp
    log_level_test.cpp
    )

target_link_libraries(log-test
    log
    unittest
    )
