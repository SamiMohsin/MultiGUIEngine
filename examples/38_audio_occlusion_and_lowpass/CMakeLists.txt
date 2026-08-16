add_executable(example_38_audio_occlusion_and_lowpass
    main.c
)

target_link_libraries(example_38_audio_occlusion_and_lowpass PRIVATE
    mg-audio
    mg-pal
    mg-alloc
    mg-math
    mg-rx
    m
)
