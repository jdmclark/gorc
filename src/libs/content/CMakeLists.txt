add_library(content STATIC
    asset.cpp
    asset_ref.cpp
    content_manager.cpp
    fourcc.cpp
    image.cpp
    json_loader.cpp
    loader.cpp
    loader_registry.cpp
    text_loader.cpp
    )

target_link_libraries(content
    boost
    io
    log
    sfml
    text
    utility
    vfs
    )

add_subdirectory(unit-test)
