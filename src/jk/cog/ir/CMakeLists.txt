add_library(cog-ir STATIC
    ir_printer.cpp
    )

target_link_libraries(cog-ir
    log
    io
    cog-vm
    )

add_subdirectory(unit-test)
