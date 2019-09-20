find_package(GTest MODULE REQUIRED)

add_executable(tests)
target_sources(tests
  PRIVATE
    main.cpp
    ${PROJECT_SOURCE_DIR}/src/Camera/Camera.cpp
    ${PROJECT_SOURCE_DIR}/src/Camera/PerspectiveCamera.cpp
)
target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(tests PRIVATE GTest::GTest GTest::Main)

add_test(AllTests tests)
