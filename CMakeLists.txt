cmake_minimum_required(VERSION 3.0)

# target name
project(advtest.rom C ASM)

enable_language(ASM)

# for debug
#set(CMAKE_VERBOSE_MAKEFILE 1)

include(./cmake/macros.cmake)

# source files
add_source_files(
    ./src/asm/psgdriver.asm
    ./src/asm/font.asm
    ./src/asm/resource.asm
    ./src/asm/unpack.asm
    ./src/asm/switch_bank.asm
    ./src/asm/message.asm
    ./src/asm/key.asm
    ./src/c/main.c
)

add_compile_flags(C
    +msx
    -I$ENV{Z88DK_HOME}/include
    -Isrc/c
    -vn
    -llib3d
    -lm
    #-lndos
    -lmsxbios
#    --list
#    -m
#    -s
    # https://github.com/z88dk/z88dk/wiki/Classic-allocation#automatic-heap-configuration
    -DAMALLOC
)

add_compile_flags(ASM
    +msx
#    --list
)

add_compile_flags(LD
    -create-app
    -subtype=rom
#    -compiler=sdcc
    -compiler=sccz80
    -pragma-define:MAPPER_ASCII16
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../dist)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
