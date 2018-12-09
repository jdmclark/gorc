add_executable(io-test
    binary_serialization_test.cpp
    input_stream_test.cpp
    memory_file_test.cpp
    output_stream_test.cpp
    )

target_link_libraries(io-test
    io
    math
    unittest
    )
