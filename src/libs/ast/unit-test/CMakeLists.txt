add_executable(ast-test
    factory_test.cpp
    node_test.cpp
    )

target_link_libraries(ast-test
    ast
    unittest
    )
