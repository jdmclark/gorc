add_library(cog-ast STATIC
    ast.cpp
    )

target_link_libraries(cog-ast
    ast
    log
    cog-script
    )
