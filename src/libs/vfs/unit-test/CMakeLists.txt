add_executable(vfs-test
    native_file_system_test.cpp
    virtual_container_iterator_test.cpp
    )

target_link_libraries(vfs-test
    unittest
    vfs
    )
