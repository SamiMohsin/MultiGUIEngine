add_executable(mg-profiler
    main.c
)

target_link_libraries(mg-profiler PRIVATE
    mg-scene
    mg-pal
)
