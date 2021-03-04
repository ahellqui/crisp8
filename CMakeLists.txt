cmake_minimum_required (VERSION 3.10.2)

set (CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project (crisp8 LANGUAGES C)

file (GLOB SOURCES crisp8/*.c crisp8/*.h)

add_library (crisp8 ${SOURCES})

set_target_properties (crisp8 PROPERTIES C_STANDARD 99)

# Compilation options. Use -D<OPTION>=<ON|OFF> to toggle from the command line
option(DISPLAY_USE_ALPHA "Use the integer value of the framebuffer as alpha" ON)

if(DISPLAY_USE_ALPHA)
    target_compile_definitions(crisp8 PRIVATE CRISP8_DISPLAY_USE_ALPHA)
endif()
