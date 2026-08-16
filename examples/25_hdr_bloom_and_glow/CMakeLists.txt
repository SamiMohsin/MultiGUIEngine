add_executable(example_25_hdr_bloom_and_glow
    main.c
)

target_link_libraries(example_25_hdr_bloom_and_glow PRIVATE
    mg-render
    mg-pal
    mg-alloc
    mg-math
    mg-rx
)
