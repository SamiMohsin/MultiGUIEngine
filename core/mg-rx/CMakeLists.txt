add_library(mg-rx STATIC
    src/mg_subscription.c
    src/mg_observable.c
    src/mg_subject.c
    src/mg_behavior_subject.c
    src/mg_operators.c
    src/mg_bus.c
)

target_include_directories(mg-rx PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(mg-rx PUBLIC
    mg-pal
    mg-alloc
)
