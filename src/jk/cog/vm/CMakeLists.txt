add_library(cog-vm STATIC
    call_stack_frame.cpp
    continuation.cpp
    default_value_mapping.cpp
    default_verbs.cpp
    executor.cpp
    executor_linkage.cpp
    heap.cpp
    instance.cpp
    pulse_record.cpp
    restart_exception.cpp
    sleep_record.cpp
    suspend_exception.cpp
    timer_record.cpp
    virtual_machine.cpp
    )

target_link_libraries(cog-vm
    cog-script
    io
    log
    )

add_subdirectory(unit-test)
