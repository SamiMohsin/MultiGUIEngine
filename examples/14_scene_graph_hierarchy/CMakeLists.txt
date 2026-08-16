add_executable(example_14_scene_graph_hierarchy
    main.c
)

target_link_libraries(example_14_scene_graph_hierarchy PRIVATE
    mg-scene
    mg-math
    mg-pal
    mg-alloc
    mg-rx
)
