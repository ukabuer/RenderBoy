add_executable(
    GLTFViewerOpenGL
    ${PROJECT_SOURCE_DIR}/apps/common/viewer.cpp
    GLTFViewerOpenGL.cpp
)
target_include_directories(GLTFViewerOpenGL PRIVATE ${PROJECT_SOURCE_DIR}/apps/common)
target_link_libraries(GLTFViewerOpenGL PRIVATE glfw RenderBoyCore)
target_compile_features(GLTFViewerOpenGL PRIVATE cxx_std_14)
