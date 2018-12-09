add_executable(boc
    main.cpp
    subcommand_type.cpp
    )

target_link_libraries(boc
    build-common
    program
    system
    text
    )
