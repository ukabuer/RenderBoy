cmake_minimum_required(VERSION 3.12)
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

find_package(SFML 2 REQUIRED COMPONENTS graphics)
find_package(TBB REQUIRED)
find_package(spdlog REQUIRED)
find_package(Eigen3 REQUIRED)

add_executable(RenderBoy)
target_sources(RenderBoy
  PRIVATE
    src/main.cpp
    src/Geometry.cpp
    src/Camera/PerspectiveCamera.cpp
    src/Renderer/Renderer.cpp
    src/ScanLine.cpp
)
target_link_libraries(RenderBoy
  PRIVATE
    sfml-graphics
    TBB::tbb
    Eigen3::Eigen
    spdlog::spdlog
)
target_include_directories(RenderBoy
  PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}
)
target_compile_features(RenderBoy PRIVATE cxx_std_14)