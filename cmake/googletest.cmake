include(FetchContent)

if(${CMAKE_VERSION} VERSION_LESS 3.14)
    include(add_FetchContent_MakeAvailable.cmake)
endif()

set(GOOGLETEST_GIT_TAG  release-1.11.0)
set(GOOGLETEST_GIT_URL  https://github.com/google/googletest.git)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY    ${GOOGLETEST_GIT_URL}
  GIT_TAG           ${GOOGLETEST_GIT_TAG}
)
  
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
