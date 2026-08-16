add_executable(example_26_dynamic_music_crossfading
    main.c
)

target_link_libraries(example_26_dynamic_music_crossfading PRIVATE
    mg-audio
    mg-pal
    mg-alloc
    mg-math
    mg-rx
    m
)
