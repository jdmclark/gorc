add_library(jk-vfs STATIC
    episode.cpp
    episode_entry.cpp
    episode_entry_type.cpp
    gob_file.cpp
    gob_virtual_container.cpp
    gob_virtual_file.cpp
    jk_virtual_file_system.cpp
    )

target_link_libraries(jk-vfs
    vfs
    )

add_subdirectory(unit-test)
