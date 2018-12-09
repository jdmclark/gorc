cmake_minimum_required(VERSION 3.8)

project(gorc)

include_directories(BEFORE src/libs src)

add_definitions(-DPLATFORM_LINUX)
set_property(GLOBAL PROPERTY CXX_STANDARD 17)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/pkg/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/pkg/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/pkg/lib")

find_package(Threads REQUIRED)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS filesystem)
add_library(boost INTERFACE)
target_link_libraries(boost INTERFACE ${Boost_LIBRARIES})

find_package(SFML 2 REQUIRED COMPONENTS graphics audio window system)
add_library(sfml INTERFACE)
target_link_libraries(sfml INTERFACE ${SFML_LIBRARIES})

find_package(OpenGL REQUIRED)

find_package(GLEW REQUIRED)
add_library(glew INTERFACE)
target_link_libraries(glew INTERFACE OpenGL::GL OpenGL::GLU ${GLEW_LIBRARIES})

add_subdirectory(src)
