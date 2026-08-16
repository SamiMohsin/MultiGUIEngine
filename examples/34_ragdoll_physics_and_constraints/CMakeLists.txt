add_executable(example_34_ragdoll_physics_and_constraints
    main.c
)

target_link_libraries(example_34_ragdoll_physics_and_constraints PRIVATE
    mg-physics
    mg-pal
    mg-alloc
    mg-math
    mg-rx
    m
)
