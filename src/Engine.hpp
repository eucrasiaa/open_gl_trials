#pragma once
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include "RenderServer.hpp"
#include "SceneElems.hpp"


class Engine {
public:
    static bool init(const char* title, int width, int height);
    static void run();
    static void shutdown();
    
    static RenderServer *renderServer;
    static bool isRunning;
    static double timeScale;

    static void addNode(Node* node);

    static std::vector<Node*> sceneNodes;
private:
    static void handleEvents();
    static void update(double dt);
    static void render(double dt);
    
    struct Resolution { 
      int width; 
      int height; 
    };
    static const Resolution resPool[4];
    static int currentResIndex;
    static void changeResolution(int direction);

};
