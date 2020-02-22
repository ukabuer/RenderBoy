find_package(Eigen3 CONFIG REQUIRED)

list(
    APPEND
    RenderBoyCore_Src
    Geometry.cpp
    Camera.cpp
    Controls/Trackball.cpp
    Context/Context.cpp
    Context/SoftwareRasterizer/Context.cpp
    Context/OpenGL/Context.cpp
    Renderer/Renderer.cpp
    Renderer/CPURasterizer/RasterizationRenderer.cpp
    Model/GLTFModelLoader.cpp
    Model/ModelLoader.cpp
    ${PROJECT_SOURCE_DIR}/third_party/glad/src/glad.c
)

add_library(
    RenderBoyCore
    STATIC
    ${RenderBoyCore_Src}
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
    ${PROJECT_SOURCE_DIR}/third_party/glad/include
)
target_compile_features(RenderBoyCore PRIVATE cxx_std_14)
