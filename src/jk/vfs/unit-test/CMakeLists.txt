add_executable(jk-vfs-test
    episode_entry_type_test.cpp
    )

target_link_libraries(jk-vfs-test
    jk-vfs
    unittest
    )
