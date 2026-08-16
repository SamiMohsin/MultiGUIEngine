add_library(mg-net STATIC
    src/mg_net.c
    src/mg_replication.c
    src/mg_prediction.c
    src/mg_bitstream.c
)


target_include_directories(mg-net PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(mg-net PUBLIC
    mg-pal
    mg-alloc
    mg-rx
    mg-math
)

