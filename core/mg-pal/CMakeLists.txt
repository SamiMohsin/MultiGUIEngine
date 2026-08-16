add_library(mg-pal STATIC
    src/mg_pal_stub.c
    src/mg_time.c
    src/mg_thread.c
    src/mg_fs.c
    src/mg_window.c
)

target_include_directories(mg-pal PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(UNIX AND NOT APPLE)
    target_link_libraries(mg-pal PUBLIC
        pthread
        rt
        dl
    )
elseif(APPLE)
    target_link_libraries(mg-pal PUBLIC
        pthread
        dl
    )
endif()
