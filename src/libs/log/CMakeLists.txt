add_library(log STATIC
    diagnostic_context.cpp
    diagnostic_context_location.cpp
    file_log_backend.cpp
    log_backend.cpp
    log.cpp
    log_frontend.cpp
    logged_runtime_error.cpp
    log_level.cpp
    log_midend.cpp
    stdio_log_backend.cpp
    )

target_link_libraries(log
    utility
    )

add_subdirectory(unit-test)
add_subdirectory(tests/example-file-log)
add_subdirectory(tests/example-stdio-log)
