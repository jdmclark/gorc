add_library(unittest STATIC
    case.cpp
    case_factory.cpp
    exception.cpp
    fixture.cpp
    main.cpp
    reporter.cpp
    stream_reporter.cpp
    strings.cpp
    suite.cpp
    suite_factory.cpp
    suite_registry.cpp
    test_log_backend.cpp
    )

target_link_libraries(unittest
    log
    )

add_subdirectory(unit-test)
add_subdirectory(tests/example-test-suite)
