add_library(mg-audio STATIC
    src/mg_audio.c
    src/mg_dsp.c
    src/mg_spatial_fx.c
    src/mg_music_sm.c
    src/mg_sfx.c
)



target_include_directories(mg-audio PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(mg-audio PUBLIC
    mg-pal
    mg-alloc
    mg-rx
    mg-math
    m
)

