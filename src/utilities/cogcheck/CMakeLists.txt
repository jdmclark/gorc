add_executable(cogcheck
    cogcheck_compiler.cpp
    disassembler.cpp
    main.cpp
    print_ast.cpp
    print_ast_visitor.cpp
    )

target_link_libraries(cogcheck
    cog-compiler
    program
    )
