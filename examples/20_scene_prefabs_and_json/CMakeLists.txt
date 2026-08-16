add_executable(example_20_scene_prefabs_and_json
    main.c
)

target_link_libraries(example_20_scene_prefabs_and_json PRIVATE
    mg-scene
    mg-pal
    mg-alloc
    mg-math
    mg-rx
)
