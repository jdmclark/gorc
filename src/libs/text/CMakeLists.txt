add_library(text STATIC
    extract_path.cpp
    generic_tokenizer.cpp
    token_helpers.cpp
    tokenizer.cpp
    tokenizer_stream.cpp
    json_input_stream.cpp
    json_output_stream.cpp
    json_tokenizer.cpp
    )

target_link_libraries(text
    io
    log
    )

add_subdirectory(unit-test)
