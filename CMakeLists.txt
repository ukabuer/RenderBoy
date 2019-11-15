cmake_minimum_required(VERSION 3.12)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

project(RenderBoy)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_path(STB_IMAGE_INCLUDE_DIR stb_image.h)
find_path(CPPTOML_INCLUDE_DIR cpptoml.h)

add_executable(RenderBoy)
target_sources(RenderBoy
  PRIVATE
    src/main.cpp
    src/Geometry.cpp
    src/Camera.cpp
    src/CameraControls/OrbitControl.cpp
    src/Material/PhongMaterial.cpp
    # src/Renderer/ScanLine.cpp
    # src/Renderer/ScanLineRenderer.cpp
    src/Renderer/RasterizationRenderer.cpp
    src/Model.cpp
    src/utils.cpp
)

target_link_libraries(RenderBoy
  PRIVATE
    glfw
    glad::glad
    Eigen3::Eigen
    assimp::assimp
)
target_include_directories(RenderBoy
  PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${ASSIMP_INCLUDE_DIRS}
    ${STB_IMAGE_INCLUDE_DIR}
    ${CPPTOML_INCLUDE_DIR}
)
target_compile_features(RenderBoy PRIVATE cxx_std_14)

enable_testing()
add_subdirectory(tests)
