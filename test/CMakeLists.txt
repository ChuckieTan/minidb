include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.11.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB test_sources
    "${PROJECT_SOURCE_DIR}/test/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)
add_executable(minidb_test "${test_sources}")
target_include_directories(minidb_test PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
)
target_link_libraries(minidb_test gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(minidb_test)
