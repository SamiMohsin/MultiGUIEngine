add_executable(example_40_input_action_remapping_and_combos
    main.c
)

target_link_libraries(example_40_input_action_remapping_and_combos PRIVATE
    mg-input
    mg-pal
    mg-alloc
    mg-math
    mg-rx
    m
)
