add_library(math INTERFACE)

target_link_libraries(math INTERFACE
    utility
    )

add_subdirectory(unit-test)
