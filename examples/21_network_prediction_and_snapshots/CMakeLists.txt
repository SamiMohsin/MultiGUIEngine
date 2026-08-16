add_executable(example_21_network_prediction_and_snapshots
    main.c
)

target_link_libraries(example_21_network_prediction_and_snapshots PRIVATE
    mg-net
    mg-pal
    mg-alloc
    mg-math
    mg-rx
)
