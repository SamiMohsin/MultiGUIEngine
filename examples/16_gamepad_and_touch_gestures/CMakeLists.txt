add_executable(example_16_gamepad_and_touch_gestures
    main.c
)

target_link_libraries(example_16_gamepad_and_touch_gestures PRIVATE
    mg-input
    mg-pal
    mg-alloc
    mg-rx
)
