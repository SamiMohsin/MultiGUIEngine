add_executable(example_09_particles_and_fsm
    main.c
)

target_link_libraries(example_09_particles_and_fsm PRIVATE
    mg-scene
    mg-render
    mg-pal
    mg-alloc
    mg-rx
)
