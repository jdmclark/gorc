add_executable(jk-content-test
    colormap_test.cpp
    )

target_link_libraries(jk-content-test
    jk-content
    unittest
    )
