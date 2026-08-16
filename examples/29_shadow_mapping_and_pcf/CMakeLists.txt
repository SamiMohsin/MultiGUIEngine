add_executable(example_29_shadow_mapping_and_pcf
    main.c
)

target_link_libraries(example_29_shadow_mapping_and_pcf PRIVATE
    mg-render
    mg-pal
    mg-alloc
    mg-math
    mg-rx
    m
)
