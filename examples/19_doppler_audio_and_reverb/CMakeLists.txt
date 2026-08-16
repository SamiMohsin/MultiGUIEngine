add_executable(example_19_doppler_audio_and_reverb
    main.c
)

target_link_libraries(example_19_doppler_audio_and_reverb PRIVATE
    mg-audio
    mg-pal
    mg-alloc
    mg-math
    mg-rx
)
