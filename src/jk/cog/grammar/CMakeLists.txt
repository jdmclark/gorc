add_library(cog-grammar STATIC
    grammar.cpp
    lexer.cpp
    parser.cpp
    )

target_link_libraries(cog-grammar
    cog-ast
    log
    text
    )
