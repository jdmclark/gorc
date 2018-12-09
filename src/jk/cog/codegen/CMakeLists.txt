add_library(cog-codegen STATIC
    codegen.cpp
    lval_expression_gen_visitor.cpp
    nonval_expression_gen_visitor.cpp
    rval_expression_gen_visitor.cpp
    statement_gen_visitor.cpp
    )

target_link_libraries(cog-codegen
    cog-ast
    cog-ir
    )
