add_executable(test_mg_scene
    test_mg_scene.c
)

target_link_libraries(test_mg_scene PRIVATE
    mg-scene
)

add_test(NAME mg-scene.unit COMMAND test_mg_scene)
