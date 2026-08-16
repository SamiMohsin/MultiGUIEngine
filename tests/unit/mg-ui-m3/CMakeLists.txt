add_executable(test_mg_ui_m3
    test_mg_ui_m3.c
)

target_link_libraries(test_mg_ui_m3 PRIVATE
    mg-ui
    mg-render
    mg-pal
    mg-alloc
    mg-rx
)

add_test(NAME mg-ui-m3.unit COMMAND test_mg_ui_m3)
