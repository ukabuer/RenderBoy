find_package(Eigen3 CONFIG REQUIRED)

add_library(
    RenderBoyCore
    STATIC
    Geometry.cpp
    Camera.cpp
    Controls/Trackball.cpp
    Renderer/RasterizationRenderer.cpp
    Model.cpp
)
target_link_libraries(
    RenderBoyCore
    PRIVATE
    Eigen3::Eigen
)
target_include_directories(
    RenderBoyCore
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/third_party/tinygltf
)
target_compile_features(RenderBoyCore PRIVATE cxx_std_14)
