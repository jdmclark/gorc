add_executable(boc-test
    find_tests.cpp
    main.cpp
    run_tests.cpp
    )

target_link_libraries(boc-test
    build-common
    log
    program
    system
    )
