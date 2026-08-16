add_executable(example_30_network_delta_compression
    main.c
)

target_link_libraries(example_30_network_delta_compression PRIVATE
    mg-net
    mg-pal
    mg-alloc
    mg-math
    mg-rx
    m
)
