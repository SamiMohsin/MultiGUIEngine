add_executable(example_13_physics_3d_and_raycast
    main.c
)

target_link_libraries(example_13_physics_3d_and_raycast PRIVATE
    mg-physics
    mg-math
    mg-pal
    mg-alloc
    mg-rx
)
