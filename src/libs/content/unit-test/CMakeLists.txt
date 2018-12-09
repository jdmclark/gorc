add_executable(content-test
    content_manager_test.cpp
    fourcc_test.cpp
    id_test.cpp
    loader_registry_test.cpp
    )

target_link_libraries(content-test
    content
    unittest
    )
