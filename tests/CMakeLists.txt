find_package(Eigen3 CONFIG REQUIRED)
include(${PROJECT_SOURCE_DIR}/third_party/catch2.cmake)

add_executable(tests)
target_sources(tests
    PRIVATE
    main.cpp
    )
target_include_directories(
    tests
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(
    tests
    PRIVATE
    RenderBoyCore
    Catch2::Catch2
    Eigen3::Eigen
)

add_executable(MathTest math.cpp)
target_link_libraries(MathTest PRIVATE Catch2::Catch2)
target_include_directories(
    MathTest
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

enable_testing()
add_test(AllTests tests)
add_test(Math MathTest)