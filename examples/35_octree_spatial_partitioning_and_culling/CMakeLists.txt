add_executable(example_35_octree_spatial_partitioning_and_culling
    main.c
)

target_link_libraries(example_35_octree_spatial_partitioning_and_culling PRIVATE
    mg-scene
    mg-pal
    mg-alloc
    mg-math
    mg-rx
    m
)
