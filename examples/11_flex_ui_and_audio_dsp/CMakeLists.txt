add_executable(example_11_flex_ui_and_audio_dsp
    main.c
)

target_link_libraries(example_11_flex_ui_and_audio_dsp PRIVATE
    mg-ui
    mg-audio
    mg-render
    mg-pal
    mg-alloc
    mg-rx
)
