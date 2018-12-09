add_executable(math-test
    almost_equal_test.cpp
    color_test.cpp
    grid_test.cpp
    point_test.cpp
    size_test.cpp
    vector_test.cpp
    )

target_link_libraries(math-test
    math
    unittest
    )
