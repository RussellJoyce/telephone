add_library(dfplayer_pico INTERFACE)

target_sources(dfplayer_pico INTERFACE
    src/dfplayer.c
    src/dfplayer_ll.c
)

target_include_directories(dfplayer_pico INTERFACE include)
