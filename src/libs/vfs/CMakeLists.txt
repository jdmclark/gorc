add_library(vfs STATIC
    native_file_system.cpp
    virtual_container_iterator.cpp
    virtual_container.cpp
    virtual_file.cpp
    virtual_file_system.cpp
    )

target_link_libraries(vfs
    io
    text
    )

add_subdirectory(unit-test)
