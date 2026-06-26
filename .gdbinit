#no shared libs
set auto-solib-add off

# Keep your code's symbols active
file ./build/OpenGL_first

# General UI/Behavior 
set disassembly-flavor intel
set print pretty on
set print frame-arguments all
set pagination off

tui enable
layout src
focus cmd
define ps
    printf "String at %p: \"%s\"\n", $arg0, (char*)$arg0
end
document ps
    print c-string.
    ex: ps my_buffer
end

define inspect_buf
    printf "Hex dump of %d bytes at %p:\n", $arg1, $arg0
    x/$arg1bx $arg0
end

define ll
    info locals
    printf "\n--- Backtrace ---\n"
    backtrace
end

break main

define hook-stop
    if $_thread
        frame
    end
end

echo \n--- Text Editor GDB Config Loaded ---\n

