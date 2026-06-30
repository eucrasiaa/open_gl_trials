#pragma once
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include "RenderServer.hpp"
#include "SceneElems.hpp"



// RenderServer *Engine::renderServer = nullptr;
// bool Engine::isRunning = false;
// double Engine::timeScale = 1.0;
// std::vector<Node*> Engine::sceneNodes;
class Engine {
  public:
    bool init(const char* title, int width, int height);
    void run();
    void shutdown();

    // RenderServer *renderServer ;
    bool isRunning = false;
    double timeScale = 1.0;



    void addNode(Node* node);

    std::vector<Node*> sceneNodes;

    static Engine& Get() {
      static Engine instance;
      return instance;
    }

  private:
    void handleEvents();
    void update(double dt);
    void render(double dt);

    struct Resolution { 
      int width; 
      int height; 
    };
    const Resolution resPool[4] = {
      { 640,  360 },
      { 1280, 720 },
      { 1600, 900 },
      { 1920, 1080 }
    };
    int currentResIndex=1;
    void changeResolution(int direction);



    Engine() = default;
    ~Engine() = default;
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
};
