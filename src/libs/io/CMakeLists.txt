add_library(io STATIC
    binary_input_stream.cpp
    binary_output_stream.cpp
    file.cpp
    input_stream.cpp
    memory_file.cpp
    native_file.cpp
    output_stream.cpp
    read_only_file.cpp
    std_input_stream.cpp
    std_output_stream.cpp
    )

target_link_libraries(io
    boost
    log
    utility
    )

add_subdirectory(unit-test)
add_subdirectory(tests/example-binary-stream)
add_subdirectory(tests/example-input-stream)
add_subdirectory(tests/example-native-file)
add_subdirectory(tests/example-output-stream)
