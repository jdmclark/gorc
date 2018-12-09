add_library(ecs STATIC
    aspect.cpp
    )

target_link_libraries(ecs
    content
    )

add_subdirectory(unit-test)
