add_library(build-common STATIC
    change_to_project_root.cpp
    make_progress_factory.cpp
    paths.cpp
    )

target_link_libraries(build-common
    io
    log
    text
    )
