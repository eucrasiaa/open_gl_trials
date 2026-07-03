cmake_minimum_required(VERSION 3.15)
project(OpenGL_first LANGUAGES C CXX)


# presets reminder:
# simply just:
#   cmake --preset sdl2-debug
#   cmake --preset raylib-release
# etc. from preset file
# then:
# cmake --build --preset sdl2-debug


# run via
# # Regular running
#./build/sdl2-debug/render_trials

# GDB running
#gdb ./build/sdl2-debug/render_trials

# Valgrind running
#valgrind --leak-check=full --suppressions=sdl2.supp ./build/sdl2-debug/render_trials

if(CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Do not build in-source! Use: cmake -B build && cmake --build build")
endif()

# setups
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# can pick a backend?
set(BACKEND "SDL2" CACHE STRING "Graphics backend to use (SDL2 or RAYLIB)")
set_property(CACHE BACKEND PROPERTY STRINGS "SDL2" "RAYLIB")

# asan valgrind maybe
option(USE_ASAN "Enable AddressSanitizer (Debug/RelWithDebInfo)" OFF)
if(USE_ASAN)
    set(USE_VALGRIND OFF CACHE BOOL "" FORCE)
else()
    option(USE_VALGRIND "Enable Valgrind target profile" ON)
endif()

# option(USE_VALGRIND "Enable Valgrind target profile" ON)

if(USE_ASAN AND USE_VALGRIND)
    message(WARNING "Both ASan and Valgrind are enabled. Do not run the 'valgrind' target on an ASan-instrumented binary!")
endif()

# explicit valuable here, when optimized
# add_executable(${PROJECT_NAME} 
#     src/main.cpp
#     # list others here!!
# )
# # the rest
# add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
# for developing, the auto is fine:
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS 
    "main.cpp"
    "src/*.cpp"
    "src/*.cc"
    "src/*.c"
)
# in case?
file(GLOB_RECURSE PROJECT_HEADERS CONFIGURE_DEPENDS
    "src/*.h"
    "src/*.hpp"
    "include/*.h"
    "include/*.hpp"
)
# idk brah
# set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

message(STATUS "--- DEBUG ---")
message(STATUS "Source files found: ${PROJECT_SOURCES}")
message(STATUS "Header files found: ${PROJECT_HEADERS}")
message(STATUS "Current Source Dir: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "------------------------")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE include)



if(BACKEND STREQUAL "SDL2")
    message(STATUS "Configuring project with SDL2 backend")


    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND MINGW)
        set(SDL2_DIR "C:/msys64/clang64/lib/cmake/SDL2" CACHE PATH "Force Clang64 SDL2" FORCE)
    endif()
    # gladslop
if(NOT TARGET glad::glad)
    # 1. Create a local target using standard alphanumeric naming
    add_library(glad STATIC "${CMAKE_CURRENT_SOURCE_DIR}/glad/src/glad.c") 
    
        set_target_properties(glad PROPERTIES 
        LINKER_LANGUAGE C
        EXTERNAL_OBJECT OFF
    )
    # 2. Add properties to the local target
    target_include_directories(glad PUBLIC 
        "${CMAKE_CURRENT_SOURCE_DIR}/glad/include"
    )
    
    find_package(OpenGL REQUIRED)
    find_package(Threads REQUIRED)

    target_link_libraries(glad PUBLIC 
        OpenGL::GL 
        Threads::Threads 
        ${CMAKE_DL_LIBS}
    )

    # 3. Create the alias so target_link_libraries(... glad::glad) works downstream
    add_library(glad::glad ALIAS glad)
endif()
    
    target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)

  #clang64 fix?
    # find_package(SDL2 REQUIRED)

    find_package(SDL2 REQUIRED CONFIG)
message(STATUS "=== SDL2 DEBUG ===")
message(STATUS "SDL2_DIR: ${SDL2_DIR}")
message(STATUS "SDL2_INCLUDE_DIRS: ${SDL2_INCLUDE_DIRS}")
if(TARGET SDL2::SDL2)
    get_target_property(SDL2_INC SDL2::SDL2 INTERFACE_INCLUDE_DIRECTORIES)
    message(STATUS "SDL2::SDL2 Include: ${SDL2_INC}")
endif()
message(STATUS "==================")


if(TARGET SDL2::SDL2)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
endif()

# This tells the compiler/linker to look for standard "main" instead of "WinMain"
if(MINGW)
  target_link_options(${PROJECT_NAME} PRIVATE "-static-libgcc" "-static-libstdc++" "-static")
    target_link_options(${PROJECT_NAME} PRIVATE "-mconsole")
# endif()
    #
    #     if(MINGW OR CYGWIN)
        target_link_libraries(${PROJECT_NAME} PRIVATE 
            setupapi
            stdc++exp
            SDL2main
            winmm
            version
            imm32
            mingw32
        )
    endif()
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_BACKEND_SDL2)
elseif(BACKEND STREQUAL "RAYLIB")
    message(STATUS "Configuring project with Raylib backend")
    find_package(raylib REQUIRED)
    
    if(TARGET raylib::raylib)
        target_link_libraries(${PROJECT_NAME} PRIVATE raylib::raylib)
    elseif(TARGET raylib)
        target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
    endif()
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_BACKEND_RAYLIB)
endif()
# for both?
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4) # eww mvsc
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:/Od;/Zi;/DDEBUG>
        $<$<CONFIG:Release>:/O2;/DNDEBUG>
    )
else() # GCC / Clang
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic) # Active for ALL configs
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:-g3;-O0;-DDEBUG>
        $<$<CONFIG:Release>:-O3;-DNDEBUG>  # consider -march=native
    )
endif()
# for both
if(USE_ASAN)
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /fsanitize=address)
        # MSVC link options for ASan are handled automatically via compiler flag
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address -fno-omit-frame-pointer)
        # target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)  
      # target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
        target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address -fsanitize=undefined)
    endif()
endif()


add_custom_target(run
    COMMAND $<TARGET_FILE:${PROJECT_NAME}>
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Executing binary ${PROJECT_NAME}"
)

if(MINGW)
    target_link_options(${PROJECT_NAME} PRIVATE "-fuse-ld=lld")
endif()

# be willing to toggle supressions. it helps tho
if(USE_VALGRIND)
    find_program(VALGRIND_PROGRAM valgrind)
    if(VALGRIND_PROGRAM)
        add_custom_target(valgrind
            COMMAND ${VALGRIND_PROGRAM} --leak-check=full --track-origins=yes --suppressions='../sdl2.supp' $<TARGET_FILE:${PROJECT_NAME}>
            DEPENDS ${PROJECT_NAME}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Running Valgrind memory analysis"
        )
        add_dependencies(valgrind ${PROJECT_NAME})
    endif()
endif()

# clang++ -std=c++23 -g3 -O0 -fno-omit-frame-pointer -fsanitize=address \
#       -DSDL_MAIN_HANDLED \
#       -I./include \
#       -I./glad/include \
#       -I/clang64/include/SDL2 \
#       src/*.cpp glad/src/glad.c \
#       -lSDL2 -lopengl32 \
#       -o asan_test.exe
#
#

# if(USE_ASAN)
#     # Define where your suppression file lives (adjust if it is named differently)
#     set(ASAN_SUPP_FILE "${CMAKE_CURRENT_SOURCE_DIR}/asan.supp")
#
#     if(EXISTS "${ASAN_SUPP_FILE}")
#         message(STATUS "Found ASan suppression file: ${ASAN_SUPP_FILE}")
#     else()
#         message(WARNING "ASan suppression file not found at ${ASAN_SUPP_FILE}. Leaks will not be filtered.")
#     endif()
#
#     add_custom_target(asan_run
#         # Use cmake -E env to set the ASAN_OPTIONS env variable cross-platform before launch
#         COMMAND ${CMAKE_COMMAND} -E env "ASAN_OPTIONS=detect_leaks=1:suppressions=${ASAN_SUPP_FILE}" $<TARGET_FILE:${PROJECT_NAME}>
#         DEPENDS ${PROJECT_NAME}
#         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#         COMMENT "Executing binary with AddressSanitizer monitoring and suppressions"
#     )
#     add_dependencies(asan_run ${PROJECT_NAME})
# endif()
# dll stuff
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS
    )
endif()
