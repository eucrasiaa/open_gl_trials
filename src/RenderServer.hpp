#pragma once
#include <glad/glad.h>
#include <SDL2/SDL.h>

class RenderServer {
public:
    static SDL_Window* window;
    static SDL_GLContext glContext;
    static bool init(const char* title, int width, int height);
    

    static int changeResolution(int width, int height);

    static void render(double dt);
    static void shutdown();
    

};
