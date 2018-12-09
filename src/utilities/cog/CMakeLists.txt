add_executable(cog
    main.cpp
    events.cpp
    read_cog_value.cpp
    scenario.cpp
    value_mapping.cpp
    )

target_link_libraries(cog
    cog-compiler
    cog-vm
    content
    jk-vfs
    program
    )
