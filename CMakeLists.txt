project(memhack)
find_program(pkg-config pkg-config)
exec_program(pkg-config ARGS "--cflags gtk+-2.0" OUTPUT_VARIABLE GTK_CFLAGS)
exec_program(pkg-config ARGS "--libs gtk+-2.0" OUTPUT_VARIABLE GTK_LDFLAGS)
set(CMAKE_C_FLAGS "-std=c99 -g -Wall ${GTK_CFLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${GTK_LDFLAGS}")
add_executable (memhack system.c callbacks.c interface.c location.c profile.c main.c support.c winaddptr.c winmain.c)
add_custom_target(tags COMMAND "./gentags")

