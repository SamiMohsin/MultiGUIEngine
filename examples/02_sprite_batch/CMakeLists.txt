add_executable(example_02_sprite_batch
    main.c
)

target_link_libraries(example_02_sprite_batch PRIVATE
    mg-render
)
