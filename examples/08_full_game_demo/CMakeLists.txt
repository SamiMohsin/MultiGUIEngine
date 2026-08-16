add_executable(example_08_full_game_demo
    main.c
)

target_link_libraries(example_08_full_game_demo PRIVATE
    mg-render
    mg-scene
    mg-physics
    mg-audio
    mg-input
    mg-ui
    mg-ai-core
    mg-pal
    mg-alloc
    mg-rx
)
