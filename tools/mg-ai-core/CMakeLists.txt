add_library(mg-ai-core STATIC
    src/mg_ai.c
)

target_include_directories(mg-ai-core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(mg-ai-core PUBLIC
    mg-rx
    mg-pal
    mg-alloc
)
