add_executable(example_32_procedural_audio_sfx
    main.c
)

target_link_libraries(example_32_procedural_audio_sfx PRIVATE
    mg-audio
    mg-pal
    mg-alloc
    mg-math
    mg-rx
    m
)
