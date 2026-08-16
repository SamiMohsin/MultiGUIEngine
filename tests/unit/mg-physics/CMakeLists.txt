add_executable(test_mg_physics
    test_mg_physics.c
)

target_link_libraries(test_mg_physics PRIVATE
    mg-physics
)

add_test(NAME mg-physics.unit COMMAND test_mg_physics)
