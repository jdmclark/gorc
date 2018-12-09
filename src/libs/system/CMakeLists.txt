add_library(system STATIC
    argument_list.cpp
    env.cpp
    pipe.cpp
    process.cpp
    self.cpp
    )

target_link_libraries(system
    boost
    io
    log
    )

add_subdirectory(unit-test)
add_subdirectory(tests/child-program)
add_subdirectory(tests/parent-program)
