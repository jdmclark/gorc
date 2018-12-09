add_library(cog-compiler STATIC
    compiler.cpp
    script_loader.cpp
    )

target_link_libraries(cog-compiler
    cog-ast
    cog-codegen
    cog-grammar
    cog-ir
    cog-semantics
    )
