add_executable(example_39_ssao_screen_space_ambient_occlusion
    main.c
)

target_link_libraries(example_39_ssao_screen_space_ambient_occlusion PRIVATE
    mg-render
    mg-pal
    mg-alloc
    mg-math
    mg-rx
    m
)
