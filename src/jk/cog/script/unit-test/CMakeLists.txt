add_executable(cog-script-test
    function_verb_test.cpp
    message_table_test.cpp
    message_type_test.cpp
    mock_verb_test.cpp
    source_type_test.cpp
    string_table_test.cpp
    symbol_table_test.cpp
    type_test.cpp
    value_test.cpp
    verb_table_test.cpp
    verb_traits_test.cpp
    )

target_link_libraries(cog-script-test
    cog-script
    unittest
    )
