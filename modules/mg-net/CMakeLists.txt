add_library(mg-net STATIC
    src/mg_net.c
)

target_include_directories(mg-net PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(mg-net PUBLIC
    mg-pal
    mg-alloc
    mg-rx
)
