add_executable(example_33_compute_shader_gpu_acceleration
    main.c
)

target_link_libraries(example_33_compute_shader_gpu_acceleration PRIVATE
    mg-render
    mg-pal
    mg-alloc
    mg-math
    mg-rx
    m
)
