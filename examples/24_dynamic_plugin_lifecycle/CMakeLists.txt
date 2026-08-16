add_executable(example_24_dynamic_plugin_lifecycle
    main.c
)

target_link_libraries(example_24_dynamic_plugin_lifecycle PRIVATE
    plugin-host
    mg-scene
    mg-pal
    mg-alloc
    mg-math
    mg-rx
)
