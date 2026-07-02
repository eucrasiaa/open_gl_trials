#!/bin/bash

set -e
cd "$(dirname "$0")"
mkdir -p build
cd build

# if [ ! -f CMakeCache.txt ] && [ "$1" != "clean" ]; then
#     cmake .. -DCMAKE_BUILD_TYPE=Debug -DUSE_ASAN=OFF
# fi

PROJECT_NAME="OpenGL_first"
BUILD_DIR="build"


BACKEND_CHOICE="SDL2"
if [ "${2,,}" = "raylib" ]; then
    BACKEND_CHOICE="RAYLIB"
fi

# TODO: fix the cd thing is there really any reason to do that LOL 

if [ ! -f CMakeCache.txt ] && [ "$1" != "clean" ]; then
    echo "No existing build configuration found. Initializing Debug (${BACKEND_CHOICE})..."
    cmake .. -DCMAKE_BUILD_TYPE=Debug -DUSE_ASAN=OFF -DBACKEND=${BACKEND_CHOICE}
fi

echo "Using: ${BACKEND_CHOICE}"

case "$1" in
    d|debug)
        cmake .. -DCMAKE_BUILD_TYPE=Debug -DUSE_ASAN=OFF -DBACKEND=${BACKEND_CHOICE}
        cmake --build .
        ;;
    f|frelease)
        cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_ASAN=OFF -DBACKEND=${BACKEND_CHOICE}
        cmake --build .
        ;;
    a|asan|sanitize)
      #rm -rf build/
      #cmake -B build -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug -DUSE_ASAN=ON -DBACKEND="SDL2"
      #cmake --build build

        cmake . -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_PREFIX_PATH=/clang64 -DCMAKE_BUILD_TYPE=Debug -DUSE_ASAN=ON -DBACKEND=${BACKEND_CHOICE}
        cmake --build .
        # cmake .. -DCMAKE_BUILD_TYPE=Debug -DUSE_ASAN=ON -DBACKEND=${BACKEND_CHOICE}
        # cmake --build .
        ;;
    r|run)
        # cmake .. -DCMAKE_BUILD_TYPE=Debug -DUSE_ASAN=OFF -DBACKEND=${BACKEND_CHOICE}
        cmake --build . #--target run
        exec ./${PROJECT_NAME} 
        # ./${PROJECT_NAME}
        ;;
    rc)
        exec ./${PROJECT_NAME}
        ;;
    gdb)
      gdb ./${PROJECT_NAME}
      ;;
    val|valgrind)
        cmake --build . --target valgrind
        ;;
    clean)
        echo "Cleaning build directory..."
        find . -mindepth 1 -delete
        ;;
    k)
      pkill ${PROJECT_NAME} 
      ;;
    *)
      echo "Usage: ./b.sh [(d)ebug| (f)inalrelease|(a)san|(r)un|(val)grind|clean] [ -> sdl2 <- |raylib]"
        echo "  r, run       - tries to run existing build -> trigger remake if needed"
        echo "  rc           - only runs last successful build"
        echo "  d, debug     - Debug build (symbols active, ASan off)"
        echo "  f, frelease  - Release build (optimized, ASan off)"
        echo "  gdb          - run with gdb"
        echo "  a, asan      - Debug build with AddressSanitizer enabled"
        echo "  run          - Run the program"
        echo "  val, valgrind - Run with valgrind memory analysis"
        echo "  clean        - Safely wipe build directory"
        echo "  k            - kill program (attempted by name)"
        ;;
esac
