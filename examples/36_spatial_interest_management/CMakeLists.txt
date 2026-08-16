add_executable(example_36_spatial_interest_management
    main.c
)

target_link_libraries(example_36_spatial_interest_management PRIVATE
    mg-net
    mg-pal
    mg-alloc
    mg-math
    mg-rx
    m
)
