add_executable(example_28_skeletal_animation_and_skinning
    main.c
)

target_link_libraries(example_28_skeletal_animation_and_skinning PRIVATE
    mg-scene
    mg-pal
    mg-alloc
    mg-math
    mg-rx
    m
)
