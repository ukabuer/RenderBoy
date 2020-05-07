add_executable(
    GLTFViewerSoftware
    ${PROJECT_SOURCE_DIR}/apps/common/viewer.cpp
    GLTFViewerSoftware.cpp
)
target_include_directories(GLTFViewerSoftware PRIVATE ${PROJECT_SOURCE_DIR}/apps/common)
target_link_libraries(GLTFViewerSoftware PRIVATE glfw RenderBoyCore -static-libstdc++)
target_compile_features(GLTFViewerSoftware PRIVATE cxx_std_14)
