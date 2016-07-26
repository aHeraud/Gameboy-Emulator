#TODO: Build rules for linux
cmake_minimum_required(VERSION 3.5)
project(Gameboy_Emulator)

set(EXECUTABLE_OUTPUT_PATH "bin/")

#Platform Detection
if(WIN32)
    if(MSVC)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            #Use 64 bit sdl2 libraries
            set(SDL2_LIB_DIR "extern/sdl2/msvc/SDL2-2.0.4/lib/x64/")

        else()
            #Use 32 bit sdl2 libraries
            set(SDL2_LIB_DIR "extern/sdl2/msvc/SDL2-2.0.4/lib/x86/")
        endif()
        set_property(GLOBAL PROPERTY USE_FOLDERS ON)
        include_directories("extern/sdl2/msvc/SDL2-2.0.4/include/")
        link_directories(${SDL2_LIB_DIR})
        file(COPY "${SDL2_LIB_DIR}SDL2.dll" DESTINATION "${CMAKE_BINARY_DIR}/${EXECUTABLE_OUTPUT_PATH}/Debug")
        file(COPY "${SDL2_LIB_DIR}SDL2.dll" DESTINATION "${CMAKE_BINARY_DIR}/${EXECUTABLE_OUTPUT_PATH}/Release")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Od /MDd")
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2 /Ob2 /Oi /Ot /GL /GS- /MD")
    endif()

    if(MINGW)
        include_directories("./extern/sdl2/mingw/SDL2-2.0.4/include/")
        link_directories("./extern/sdl2/mingw/SDL2-2.0.4/lib/x64/")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Werror")
    endif()
endif()

if(UNIX)
    message(FATAL_ERROR "No unix build rules")
endif()

set(SOURCE_FILES
    src/cpu.c
    src/cpu.h
    src/emulator.c
    src/emulator.h
    src/extended_instructions.c
    src/gpu.c
    src/gpu.h
    src/instructions.c
    src/instructions.h
    src/joypad.c
    src/joypad.h
    src/mbc1.c
    src/mbc1.h
    src/mbc2.c
    src/mbc2.h
    src/mbc3.c
    src/mbc3.h
    src/mbc5.c
    src/mbc5.h
    src/mmu.c
    src/mmu.h
    src/register16.c
    src/register16.h
    src/register8.c
    src/register8.h
    src/serial.c
    src/serial.h
    src/timer.c
    src/timer.h
    src/win_main.c
    src/window.c
    src/window.h)

add_executable(gameboy ${SOURCE_FILES})
target_link_libraries(gameboy "SDL2.lib" "SDL2main.lib")
install(TARGETS gameboy DESTINATION bin)