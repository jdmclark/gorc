add_executable(cog-vm-test
    heap_test.cpp
    sleep_record_test.cpp
    virtual_machine_test.cpp
    )

target_link_libraries(cog-vm-test
    cog-vm
    unittest
    )
