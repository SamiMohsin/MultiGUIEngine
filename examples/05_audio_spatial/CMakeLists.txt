add_executable(example_05_audio_spatial
    main.c
)

target_link_libraries(example_05_audio_spatial PRIVATE
    mg-audio
)
