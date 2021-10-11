include(FetchContent)

if(${CMAKE_VERSION} VERSION_LESS 3.14)
    include(add_FetchContent_MakeAvailable.cmake)
endif()

set(SPDLOG_GIT_TAG  v1.9.2)
set(SPDLOG_GIT_URL  https://github.com/gabime/spdlog.git)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY    ${SPDLOG_GIT_URL}
  GIT_TAG           ${SPDLOG_GIT_TAG}
)

FetchContent_MakeAvailable(spdlog)