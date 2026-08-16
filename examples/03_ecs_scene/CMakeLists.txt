add_executable(example_03_ecs_scene
    main.c
)

target_link_libraries(example_03_ecs_scene PRIVATE
    mg-scene
)
