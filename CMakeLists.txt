cmake_minimum_required(VERSION 3.15)
project(OpenGL_first LANGUAGES CXX)


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
option(USE_VALGRIND "Enable Valgrind target profile" ON)

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



message(STATUS "--- DEBUG ---")
message(STATUS "Source files found: ${PROJECT_SOURCES}")
message(STATUS "Header files found: ${PROJECT_HEADERS}")
message(STATUS "Current Source Dir: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "------------------------")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE include)



if(BACKEND STREQUAL "SDL2")
    message(STATUS "Configuring project with SDL2 backend")

    # gladslop
    if(NOT TARGET glad::glad)
      set(GLAD_INCLUDE_DIR "/usr/local/include")
      set(GLAD_LIBRARY "/usr/local/lib/libglad.a")
      add_library(glad::glad STATIC IMPORTED)
      set_target_properties(glad::glad PROPERTIES
          INTERFACE_INCLUDE_DIRECTORIES "${GLAD_INCLUDE_DIR}"
          IMPORTED_LOCATION "${GLAD_LIBRARY}"
      )
      find_package(OpenGL REQUIRED)
      find_package(Threads REQUIRED)

      target_link_libraries(glad::glad INTERFACE 
          OpenGL::GL 
          Threads::Threads 
          ${CMAKE_DL_LIBS} # -ldl? 
      )
    endif()
    
    target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)


    find_package(SDL2 REQUIRED)

    if(TARGET SDL2::SDL2)
        target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
        # Safely check and link SDL2::main if it exists on this platform
        if(TARGET SDL2::main)
            target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::main)
        endif()
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})
        target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
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
        target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
        target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address -fsanitize=undefined)
    endif()
endif()


add_custom_target(run
    COMMAND $<TARGET_FILE:${PROJECT_NAME}>
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Executing binary ${PROJECT_NAME}"
)



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


