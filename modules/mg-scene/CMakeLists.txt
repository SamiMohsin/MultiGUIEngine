add_library(mg-scene STATIC
    src/mg_ecs.c
    src/mg_gameplay.c
)


target_include_directories(mg-scene PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(mg-scene PUBLIC
    mg-pal
    mg-alloc
    mg-math
    mg-rx
)
