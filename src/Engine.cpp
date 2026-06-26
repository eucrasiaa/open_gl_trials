#include "Engine.hpp"
#include <SDL_error.h>
#include <glad/glad.h>
SDL_Window* Engine::window = nullptr;
SDL_GLContext Engine::glContext = nullptr;
bool Engine::isRunning = false;
double Engine::timeScale = 1.0;
std::vector<Node*> Engine::sceneNodes;

bool Engine::init(const char* title, int width, int height) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr<<"SDL Init Error: %s\n"<<SDL_GetError()<<"\n";
    return false;
  }

  // // Set core OpenGL context parameters before window generation
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  // stability
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
  // Color! Framebuff fun
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
  // anti alias
  //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

  // debug
  // glGetError();
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
  //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
  window = SDL_CreateWindow(title, 
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
      width, height, 
      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALWAYS_ON_TOP);
  if (!window) {
    std::cerr<<"Window Error: %s\n"<<SDL_GetError()<<"\n";
    return false;
  }

  glContext = SDL_GL_CreateContext(window);
  if (!glContext) {
    std::cerr<<"OpenGL Context Error: %s\n"<<SDL_GetError()<<"\n";
    return false;
  }
  if(!SDL_GL_MakeCurrent(window, glContext)){

  }

  //init GLAD
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    std::cerr<<"Failed to initialize Glad: most recent SDL Err: %s\n"<<SDL_GetError()<<"\n";
    return -1;
  }
  // 0 = vsync off, 1 = vsync, -1 = adaptive vsync (?)
  SDL_GL_SetSwapInterval(1); 

  glViewport(0,0,width,height);
  isRunning = true;
  

  // sys info 
  std::cout<< "-Vendor "            << glGetString(GL_VENDOR)    << 
              "\n-Renderer "        << glGetString(GL_RENDERER) <<
              "\n-Version "         << glGetString(GL_VERSION)  <<
              "\n-Shading Language "<< glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  // glBindBuffer()
  // glBindBufer
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

    // tell OS
    SDL_SetWindowSize(window, target.width, target.height);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    glViewport(0,0,target.width, target.height);
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

    render(elapsedTime);

    // swap back to front
    SDL_GL_SwapWindow(window);
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

  if (glContext) SDL_GL_DeleteContext(glContext);
  if (window) SDL_DestroyWindow(window);
  SDL_Quit();
  if (glContext) SDL_GL_DeleteContext(glContext);
  if (window) SDL_DestroyWindow(window);
  SDL_Quit();
}
const Engine::Resolution Engine::resPool[4] = {
  { 640,  360 },
  { 1280, 720 },
  { 1600, 900 },
  { 1920, 1080 }
};
int Engine::currentResIndex = 1;

