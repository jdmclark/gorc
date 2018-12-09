add_library(jk-content STATIC
    colormap.cpp
    colormap_loader.cpp
    inventory.cpp
    inventory_bin.cpp
    inventory_loader.cpp
    material.cpp
    material_loader.cpp
    raw_material.cpp
    renderer_object_factory.cpp
    )

target_link_libraries(jk-content
    content
    cog-compiler
    math
    )

add_subdirectory(unit-test)
