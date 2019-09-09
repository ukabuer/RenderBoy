cmake_minimum_required(VERSION 3.12)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

project(RenderBoy)

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

find_package(SFML CONFIG REQUIRED graphics)
find_package(TBB CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_path(STB_IMAGE_INCLUDE_DIR stb_image.h)

add_executable(RenderBoy)
target_sources(RenderBoy
  PRIVATE
    src/main.cpp
    src/Geometry.cpp
    src/Camera/Camera.cpp
    src/Camera/PerspectiveCamera.cpp
    src/Renderer/ScanLineRenderer.cpp
    src/Renderer/RasterizationRenderer.cpp
    src/Renderer/ScanLine.cpp
    src/Model.cpp
)
target_link_libraries(RenderBoy
  PRIVATE
    sfml-graphics
    TBB::tbb
    Eigen3::Eigen
    ${ASSIMP_LIBRARIES}
)
target_include_directories(RenderBoy
  PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${ASSIMP_INCLUDE_DIRS}
    ${STB_IMAGE_INCLUDE_DIR}
)
target_compile_features(RenderBoy PRIVATE cxx_std_14)

enable_testing()
add_subdirectory(tests)
