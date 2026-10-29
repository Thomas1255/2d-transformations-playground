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

# ---------------- math2d (pure-matrix library) ----------------
add_library(math2d STATIC src/math/Transform2D.cpp)
target_include_directories(math2d PUBLIC ${CMAKE_SOURCE_DIR}/src)

# ---------------- The app ----------------
add_executable(TransformPlayground
    src/main.cpp
    src/app/CanvasDrawing.cpp) # executable
target_include_directories(TransformPlayground PRIVATE src)
target_link_libraries(TransformPlayground PRIVATE imgui math2d)

# ---------------- tests (Google Test, run through ctest) ----------------
# find_package = "ask vcpkg whether gtest is in the manifest and give us its
# usage info." This defines the GTest::gtest / GTest::gtest_main targets.
find_package(GTest CONFIG REQUIRED)

# Unit-test executable. Only links math2d (pure library, no GL/ImGui needed),
# which is exactly why the math lives in its own lib at all.
# gtest_main supplies main(); we never write one.
add_executable(TransformTests tests/Transform2DTests.cpp)
target_include_directories(TransformTests PRIVATE src)
target_link_libraries(TransformTests PRIVATE math2d GTest::gtest_main)

# Register with CMake's test runner (ctest ships with CMake itself, nothing to
# install). Without add_test, ctest has no idea this executable is a test.
enable_testing()                                       # discovers add_test calls in this dir
add_test(NAME TransformTests COMMAND TransformTests)   # how ctest invokes it  