add_library(mg-alloc STATIC
    src/mg_arena.c
    src/mg_pool.c
    src/mg_ring_buffer.c
)

target_include_directories(mg-alloc PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(mg-alloc PUBLIC
    mg-pal
)
