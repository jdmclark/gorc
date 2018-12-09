add_executable(text-test
    generic_tokenizer_test.cpp
    token_helpers_test.cpp
    json_serialization_test.cpp
    json_specification_test.cpp
    json_tokenizer_test.cpp
    )

target_link_libraries(text-test
    text
    unittest
    )
