#TODO: Build rules for linux
cmake_minimum_required(VERSION 3.5)
project(Gameboy_Emulator)

SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "limited configs" FORCE) #Disable unneeded configurations
set(EXECUTABLE_OUTPUT_PATH "bin/")

#Platform Detection
if(WIN32)
    if(MSVC)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            #Use 64 bit sdl2 libraries
            set(SDL2_LIBRARY_DIR "extern/msvc/SDL2-2.0.4/lib/x64")
        else()
            #Use 32 bit sdl2 libraries
            set(SDL2_LIBRARY_DIR "extern/msvc/SDL2-2.0.4/lib/x86")
        endif()
        set(SDL2_INCLUDE_DIR "extern/msvc/SDL2-2.0.4/include")
        set_property(GLOBAL PROPERTY USE_FOLDERS ON)
        set(SDL2_LIBS "SDL2.lib" "SDL2main.lib")
        file(COPY "${SDL2_LIBRARY_DIR}/SDL2.dll" DESTINATION "${CMAKE_BINARY_DIR}/${EXECUTABLE_OUTPUT_PATH}/Debug")
        file(COPY "${SDL2_LIBRARY_DIR}/SDL2.dll" DESTINATION "${CMAKE_BINARY_DIR}/${EXECUTABLE_OUTPUT_PATH}/Release")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Od /MDd")
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2 /Ob2 /Oi /Ot /GL /GS- /MD")

    elseif(MINGW)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            #Use 64 bit sdl2 libraries
            set(SDL2_LIBRARY_DIR "extern/mingw/SDL2-2.0.4/x86_64-w64-mingw32/lib")
            set(SDL2_INCLUDE_DIR "extern/mingw/SDL2-2.0.4/i686-w64-mingw32/include/SDL2")
        else()
            #Use 32 bit sdl2 libraries
            set(SDL2_LIBRARY_DIR "extern/mingw/SDL2-2.0.4/i686-w64-mingw32/lib")
            set(SDL2_INCLUDE_DIR "extern/mingw/SDL2-2.0.4/x86_64-w64-mingw32/include/SDL2")
        endif()
        set(SDL2_LIBS "mingw32" "SDL2main" "SDL2")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Werror")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
        file(COPY "${SDL2_LIBRARY_DIR}/../bin/SDL2.dll" DESTINATION "${CMAKE_BINARY_DIR}")
    else()
        message(FATAL_ERROR "Invalid build settings (Use MSVC or MinGW)")
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

include_directories(${SDL2_INCLUDE_DIR})
link_directories(${SDL2_LIBRARY_DIR})

add_executable(Gameboy_Emulator ${SOURCE_FILES})
target_link_libraries(Gameboy_Emulator ${SDL2_LIBS})