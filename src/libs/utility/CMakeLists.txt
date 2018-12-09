add_library(utility STATIC
    event_bus.cpp
    file_progress.cpp
    global.cpp
    local.cpp
    progress.cpp
    rate_limited_progress.cpp
    service_registry.cpp
    shell_progress.cpp
    strcat.cpp
    string_search.cpp
    string_view.cpp
    time.cpp
    uncopyable.cpp
    wrapped.cpp
    )

target_link_libraries(utility
    Threads::Threads
    )

add_subdirectory(unit-test)
