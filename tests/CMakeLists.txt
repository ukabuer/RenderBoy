find_package(Eigen3 CONFIG REQUIRED)

add_executable(tests)
target_sources(tests
    PRIVATE
    main.cpp
    )
target_include_directories(
    tests
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/third_party/Catch2/single_include/
)
target_link_libraries(
    tests
    PRIVATE
    RenderBoyCore
    Eigen3::Eigen
)

enable_testing()
add_test(AllTests tests)
