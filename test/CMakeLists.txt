include(GoogleTest)
include(googletest)

file(GLOB test_sources
    "${PROJECT_SOURCE_DIR}/test/*.cpp"
    "${PROJECT_SOURCE_DIR}/test/util/*.cpp"
    "${PROJECT_SOURCE_DIR}/test/parser/*.cpp"
    "${PROJECT_SOURCE_DIR}/test/ast/*.cpp"
)

add_executable(minidb_test "${test_sources}")

target_include_directories(minidb_test PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
)
target_link_libraries(minidb_test PRIVATE
    gtest
    gtest_main
    spdlog
    minidb::util
    minidb::parser
    # minidb::ast
)

gtest_discover_tests(minidb_test)