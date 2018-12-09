add_executable(test-test
    mock_reporter.cpp
    test_assert_test.cpp
    test_case_order.cpp
    test_expect_test.cpp
    test_framework_test.cpp
    test_strings_test.cpp
    )

target_link_libraries(test-test
    unittest
    )
