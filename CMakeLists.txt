cmake_minimum_required(VERSION 3.15)
project(bootswap)

set(CMAKE_C_STANDARD 90)

include_directories($ENV{CEDEV}/include)

add_custom_target(Make ALL $(MAKE) WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# A dummy target to make CLion do stuff
add_executable(dummy
    src/main.c src/versions.c src/versions.h src/asm/ports.h src/asm/flash.h src/flash.c src/flash.h)
