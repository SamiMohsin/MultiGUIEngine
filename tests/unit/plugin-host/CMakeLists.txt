add_executable(test_plugin_host
    test_plugin_host.c
)

target_link_libraries(test_plugin_host PRIVATE
    plugin-host
)

add_test(NAME plugin-host.unit COMMAND test_plugin_host)
