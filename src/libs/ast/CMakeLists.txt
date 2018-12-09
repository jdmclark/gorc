add_library(ast STATIC
    factory.cpp
    node.cpp
    variant_location_visitor.cpp
    )

target_link_libraries(ast
    log
    )

add_subdirectory(unit-test)
