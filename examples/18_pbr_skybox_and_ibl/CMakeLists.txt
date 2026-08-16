add_executable(example_18_pbr_skybox_and_ibl
    main.c
)

target_link_libraries(example_18_pbr_skybox_and_ibl PRIVATE
    mg-render
    mg-pal
    mg-alloc
    mg-math
    mg-rx
)
