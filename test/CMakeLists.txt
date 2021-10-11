include(GoogleTest)
include(googletest)

file(GLOB test_sources
    "${PROJECT_SOURCE_DIR}/test/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)
add_executable(minidb_test "${test_sources}")
target_include_directories(minidb_test PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
)
target_link_libraries(minidb_test PRIVATE gtest gtest_main spdlog)

gtest_discover_tests(minidb_test)