# kinda more a temp work section.
## i plan to eventually merge up to the SDL2 Game Engine
using this to learn OpenGL

## currently functioning: 
- Basic 3D Space Rendering! 
    - 2D textuers can be placed into the world 
    - 3D objects via .OBJ, and UV texturing support!!
- Pipeline Layers allow for different types of processing:
    - standard world-space Opaque + Transparent elements
    - NEW!: UI / Camera bound 3D+2D with basic anchoring system 
    - 2 Location PostProcess pass support 
        - World Space (Pre-UI)+ Screen Space (Post UI)
- Dynamic Insertion of objects:
    - Node structures are built and attached to the main scene tree 
        - somewhat. working Parent-child transformation sytem for parenting 
- Somewhat Interactive!!
    - Camera Movement + Mouse looking within the enviroment


## CREDITS!! 
some chunks of code, notibly debug print helpers, stem from using tutorials, internet fourms, and google sources. I try to alter and rewrite most code i encounter, but some sections heavily reference known works. 



### includes and tools used:
see LICENSE for explit licensing conditions

1. Screen, Windowing, and OS Event handling (keys, mouse, etc.) handled thanks to Simple DirectMedia Layer (SDL2)
    - [SDL2](https://github.com/libsdl-org/SDL)

2. Loading and Processing of the .OBJ format uses TinyObjLoader 
    - [Tiny Loader Github](https://github.com/tinyobjloader/tinyobjloader) 

3. glad (glad1) as my extension OpenGL Loader-Generator 
    - [glad](https://github.com/Dav1dde/glad)

4. Image loading with STB / stb_img
    - [STB](https://github.com/nothings/stb)

5. Matrix and vector math aided with GLM 
    - [GLM](https://github.com/g-truc/glm)
