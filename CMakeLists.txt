cmake_minimum_required(VERSION 3.13)

project(SgIslands)

set(CMAKE_CXX_STANDARD 17)

set(
        SOURCE
        SgIslands/vendor/tinyxml2/tinyxml2.cpp
        SgIslands/vendor/imGui/imgui.cpp
        SgIslands/vendor/imGui/imgui-SFML.cpp
        SgIslands/vendor/imGui/imgui_widgets.cpp
        SgIslands/vendor/imGui/imgui_demo.cpp
        SgIslands/vendor/imGui/imgui_draw.cpp
        SgIslands/src/Main.cpp
)

add_executable(SgIslands ${SOURCE})

target_include_directories(SgIslands PUBLIC SgIslands/vendor/)

set(SFML_STATIC_LIBRARIES TRUE)
target_link_libraries(SgIslands sfml-graphics sfml-window sfml-system opengl32)

