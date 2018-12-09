add_executable(cog-ir-test
    ir_printer_test.cpp
    )

target_link_libraries(cog-ir-test
    cog-ir
    unittest
    )
