add_executable(test_mg_ui
    test_mg_ui.c
)

target_link_libraries(test_mg_ui PRIVATE
    mg-ui
    mg-render
)

add_test(NAME mg-ui.unit COMMAND test_mg_ui)
