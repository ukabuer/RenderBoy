find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

add_executable(
    RenderBoyViewer
    viewer/main.cpp
)
target_link_libraries(
    RenderBoyViewer
    PRIVATE
    RenderBoyCore
    glad::glad
    glfw
)
target_include_directories(
    RenderBoyViewer
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/viewer
)
target_compile_features(
    RenderBoyViewer
    PRIVATE
    cxx_std_14
)

add_executable(
    RenderBoyOpenGL
    gl/main.cpp
)
target_link_libraries(
    RenderBoyOpenGL
    PRIVATE
    RenderBoyCore
    glad::glad
    glfw
)
target_include_directories(
    RenderBoyOpenGL
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gl
)
target_compile_features(
    RenderBoyOpenGL
    PRIVATE
    cxx_std_14
)