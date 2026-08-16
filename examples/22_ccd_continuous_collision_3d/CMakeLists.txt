add_executable(example_22_ccd_continuous_collision_3d
    main.c
)

target_link_libraries(example_22_ccd_continuous_collision_3d PRIVATE
    mg-physics
    mg-pal
    mg-alloc
    mg-math
    mg-rx
)
