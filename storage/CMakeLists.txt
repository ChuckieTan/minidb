project(storage)

file(GLOB parser_storage
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_library(${PROJECT_NAME} ${parser_storage})
add_library(minidb::storage ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC spdlog::spdlog
    minidb::ast
    minidb::operate
)