cmake_minimum_required(VERSION 3.12)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif ()

project(RenderBoy)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

add_subdirectory(src)

option(RB_BUILD_APPS "whether to build apps" ON)
if (RB_BUILD_APPS)
  add_subdirectory(apps)
endif ()

option(RB_BUILD_TESTS "whether to build tests" ON)
if (RB_BUILD_TESTS)
  add_subdirectory(tests)
endif ()
