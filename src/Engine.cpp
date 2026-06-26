#include "Engine.hpp"
#include "RenderServer.hpp"
#include <SDL_error.h>
#include <glad/glad.h>

RenderServer *Engine::renderServer = nullptr;
bool Engine::isRunning = false;
double Engine::timeScale = 1.0;
std::vector<Node*> Engine::sceneNodes;

bool Engine::init(const char* title, int width, int height) {
  renderServer = new RenderServer();
  if(!renderServer->init(title, width, height)){
    std::cerr<<"render server failed to initialize!\n";
    return false;
  }
  isRunning = true;
  return true;
}

void Engine::handleEvents() {
  SDL_Event event;
  while (SDL_PollEvent(&event) != 0) {
    if (event.type == SDL_QUIT) {
      isRunning = false;
      shutdown();
    }
    else if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
        case SDLK_LEFT:
          changeResolution(-1); // Cycle down
          break;
        case SDLK_RIGHT:
          changeResolution(1);  // Cycle up
          break;
      }
    }
  }
}

void Engine::changeResolution(int direction) {
  int newIndex = currentResIndex + direction;

  if (newIndex >= 0 && newIndex < 4) {
    currentResIndex = newIndex;
    Resolution target = resPool[currentResIndex];
    if (renderServer->changeResolution(target.width, target.height) < 0){
      std::cerr<< " failed to change resolution... uh oh!\n";
    }
    // tell OS
    std::cout<<"Resolution changed to:"<< target.width<<", "<<target.height<<"\n";
  }
}


void Engine::update(double dt) {
  if (sceneNodes.size() > 1) {
    Node* player = sceneNodes[1];

    float speed = 150.0f;
    player->position.x += speed * static_cast<float>(dt);
  }
}


void Engine::run() {
  double accumulator = 0.0;
  const double FIXED_DT = 1.0 / 60.0; 
  const double MAX_DELTA = 0.25;

  double elapsedTime = 0.0;
  uint64_t frequency = SDL_GetPerformanceFrequency();
  uint64_t currentTicks = SDL_GetPerformanceCounter();
  while (isRunning) {
    uint64_t newTicks = SDL_GetPerformanceCounter();
    double frameTime = static_cast<double>(newTicks - currentTicks) / frequency;
    currentTicks = newTicks;

    if (frameTime > MAX_DELTA) {
      frameTime = MAX_DELTA;
    }
    elapsedTime += (frameTime * timeScale);
    accumulator += (frameTime * timeScale);

    handleEvents();

    while (accumulator >= FIXED_DT) {
      update(FIXED_DT);
      accumulator -= FIXED_DT;
    }
    render(elapsedTime); // loads renderServer if needed

    renderServer->render(elapsedTime);
  }
}
void Engine::addNode(Node* node) {
  if (node != nullptr) {
    sceneNodes.push_back(node);
  }
}

void Engine::shutdown() {
  for (auto* node : sceneNodes) {
    delete node;
  }
  sceneNodes.clear();
  renderServer->shutdown();
}
const Engine::Resolution Engine::resPool[4] = {
  { 640,  360 },
  { 1280, 720 },
  { 1600, 900 },
  { 1920, 1080 }
};
int Engine::currentResIndex = 1;

