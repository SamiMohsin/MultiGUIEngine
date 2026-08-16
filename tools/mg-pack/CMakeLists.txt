add_executable(mg-pack
    main.c
)

target_link_libraries(mg-pack PRIVATE
    mg-pal
)
