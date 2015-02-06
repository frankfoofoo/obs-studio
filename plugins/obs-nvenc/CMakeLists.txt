project(obs-nvenc)

#find_package(Libx264 REQUIRED)
#include_directories(${LIBX264_INCLUDE_DIRS})
#add_definitions(${LIBX264_DEFINITIONS})

include_directories(libnvenc)

set(obs-nvenc_SOURCES
	obs-nvenc.c
	obs-nvenc-plugin-main.c)

add_library(obs-nvenc MODULE
	${obs-nvenc_SOURCES})
target_link_libraries(obs-nvenc
	libobs)

install_obs_plugin_with_data(obs-nvenc data)
