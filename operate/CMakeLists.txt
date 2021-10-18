project(operate)

file(GLOB operate_sources
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_library(${PROJECT_NAME} ${operate_sources})
add_library(minidb::operate ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    spdlog::spdlog
    minidb::ast
    minidb::storage
)