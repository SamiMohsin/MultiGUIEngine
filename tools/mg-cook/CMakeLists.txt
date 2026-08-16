add_executable(mg-cook
    main.c
)

target_link_libraries(mg-cook PRIVATE
    mg-pal
)
