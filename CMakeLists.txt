cmake_minimum_required(VERSION 3.20)
project(TransformPlayground LANGUAGES CXX) # sets language to C++

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 CONFIG REQUIRED)   # window/input lib from vcpkg
find_package(glad CONFIG REQUIRED)    # OpenGL loader from vcpkg

# ---------------- ImGui (vendored in third_party/) ----------------
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/third_party/imgui)

add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${IMGUI_DIR}/misc/cpp
)
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
target_link_libraries(imgui PUBLIC glfw glad::glad)

# ---------------- The app ----------------
add_executable(TransformPlayground src/main.cpp)                      # executable
target_include_directories(TransformPlayground PRIVATE src)
target_link_libraries(TransformPlayground PRIVATE imgui)      