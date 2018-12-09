add_library(cog-script STATIC
    constant_table.cpp
    function_verb.cpp
    message_table.cpp
    message_type.cpp
    mock_verb.cpp
    script.cpp
    source_type.cpp
    string_table.cpp
    symbol.cpp
    symbol_table.cpp
    type.cpp
    value.cpp
    verb.cpp
    verb_table.cpp
    )

target_link_libraries(cog-script
    content
    io
    log
    math
    utility
    )

add_subdirectory(unit-test)
