#include "Engine.hpp"
#include "AxisGuide.hpp"
#include "RenderServer.hpp"
#include <SDL_error.h>
#include <glad/glad.h>
#include "Defines.h"
#include "UI3DMeshNode.hpp"
#include "UiNode.hpp"
#include "wtypes/Vec2.hpp"
bool Engine::init(const char* title, int width, int height) {
  if(!RenderServer::Get().init(title, width, height)){
    // if(!renderServer->init(title, width, height)){
    std::cerr<<"render server failed to initialize!\n";
    return false;
  }
  isRunning = true;


  UINode* UI_ELEM_2d = new UINode();
  UI_ELEM_2d->init("../assets/textures/_/_lain.jpg", Vec2(250.0f,250.0f),Vec2(0.0f,0.0f), UIAnchor::Center,UIPivot::Center);
  devNode = UI_ELEM_2d;
  // devNode->position.y+=5.0f;
  // UI_ELEM_2d->position.z-=5.0f;
  UINode* UI_ELEM_2d_2 = new UINode();
  UI_ELEM_2d_2->init("../assets/textures/_/_lain2.jpg", Vec2(100.0f,100.0f),Vec2(0.0f,0.0f), UIAnchor::TopRight,UIPivot::Center);
  // UI_ELEM_2d_2->positio;
  UI_ELEM_2d->addChild(UI_ELEM_2d_2);
    Engine::Get().addNode(UI_ELEM_2d);
  devNode_2 = UI_ELEM_2d_2;
  // UI_ELEM_2d_2->scale*=10;
  // UI_ELEM_2d->scale*=10;
  return true;
  }


  void Engine::DevAnchorRot(int direction, UINode * d) {
    int totalAnchors = 9; 
    int currentAnchorIndex = static_cast<int>(d->anchor);
    currentAnchorIndex = (currentAnchorIndex + direction) % totalAnchors;
    if (currentAnchorIndex < 0) {
      currentAnchorIndex += totalAnchors;
    }
    d->anchor = static_cast<UIAnchor>(currentAnchorIndex);
    d->isDirty = true;
  }


  void Engine::DevPivotRot(int direction, UINode * d) {
    int totalPivots = 3; 
    int currentPivotIndex = static_cast<int>(d->pivot);
    currentPivotIndex = (currentPivotIndex + direction) % totalPivots;
    if (currentPivotIndex < 0) {
      currentPivotIndex += totalPivots;
    }
    d->pivot = static_cast<UIPivot>(currentPivotIndex);
    d->isDirty = true; 
  }
  void Engine::ShaderChange(int direction) {
    int totalRes = Shaders.size();

    currentResIndex = (currentResIndex + direction) % totalRes;

    if (currentResIndex < 0) {
      currentResIndex += totalRes;
    }

    RenderServer::Get().PP = Shaders[currentResIndex];

    std::cout << "Shader changed to: " << Shaders[currentResIndex] << std::endl;
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
            ShaderChange(-1); 
                              // changeResolution(-1); // Cycle down
            break;
          case SDLK_RIGHT:
            ShaderChange(1);  
                              // changeResolution(1);  // Cycle up
            break;
          case SDLK_SPACE:
            inputState.ToggleMouse = !inputState.ToggleMouse;
            SDL_SetRelativeMouseMode(inputState.ToggleMouse? SDL_TRUE : SDL_FALSE);
            break;
          case SDLK_COMMA: // ',' Key
            if (event.key.keysym.mod & KMOD_SHIFT) {
              DevPivotRot(-1,devNode);
            } else {
              DevAnchorRot(-1,devNode);
            }

            break;
          case SDLK_PERIOD: // '.' Key
            if (event.key.keysym.mod & KMOD_SHIFT) {
              DevPivotRot(1,devNode_2);
            } else {
              DevAnchorRot(1,devNode_2);
            }
            break;

        }
      }
      else if (inputState.ToggleMouse){ 
        if(event.type == SDL_MOUSEMOTION) {
          yaw   += event.motion.xrel * mouseSensitivity;
          pitch -= event.motion.yrel * mouseSensitivity; 
          float maxPitch = glm::radians(89.0f);
          if (pitch > maxPitch)  pitch = maxPitch;
          if (pitch < -maxPitch) pitch = -maxPitch;
        }
      }
    }
    MoveCamera();
  }
#include <glm/gtx/string_cast.hpp>
#include "ansi_control.hpp"
  void Engine::MoveCamera(){

    glm::vec3 cameraPos = RenderServer::Get().cameraPos;
    glm::vec3 cameraLook = RenderServer::Get().cameraLook;
    glm::vec3 cameraUp = RenderServer::Get().cameraUp;
    //  finding forward direction
    glm::vec3 front;
    front.x = cos(yaw) * cos(pitch);

    front.y = sin(pitch);
    front.z = sin(yaw) * cos(pitch);
    front = glm::normalize(front);
    cameraLook = cameraPos + (front*100.0f);
    glm::vec3 right   = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));

    cameraUp = glm::normalize(glm::cross(right, front));
    // cameraUp = glm::normalize(glm::cross(right, glm::vec3(0.0f,1.0f,0.0f)));

#ifdef CAMERA_DEBUG

    term::move_down_c(9);
    term::move_up(9);

    std::cout<<"\nLook@"<< glm::to_string(cameraLook)<<std::endl;
    std::cout<<"From@"<< glm::to_string(cameraPos)<<std::endl;

    std::cout<<"Up@"<< glm::to_string(cameraUp)<<std::endl;
    std::cout<<"\tRoll@"<<roll<< "\n\tYaw:"<< yaw<<"\n\tPitch:"<<pitch<<std::endl;
    std::cout<<"________________________________\n"<<std::endl;
    // term::move_up(9,true);
#endif


    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL); 
    // to make camera turn matter
    if (currentKeyStates[SDL_SCANCODE_W]) {
      cameraPos += front * movementSpeed; 
    }
    if (currentKeyStates[SDL_SCANCODE_S]) {
      cameraPos -= front * movementSpeed; 
    }
    // to make camera turn matter
    if (currentKeyStates[SDL_SCANCODE_A]) {
      cameraPos -= right * movementSpeed; 
    }
    if (currentKeyStates[SDL_SCANCODE_D]) {
      cameraPos += right * movementSpeed; 
    }
    // just on true Y
    if (currentKeyStates[SDL_SCANCODE_Q]) {
      cameraPos.y += movementSpeed;
    }

    if (currentKeyStates[SDL_SCANCODE_E]) {
      cameraPos.y -= movementSpeed; 
    }

    RenderServer::Get().cameraPos  = cameraPos;
    RenderServer::Get().cameraLook = cameraLook;
    RenderServer::Get().cameraUp   = cameraUp;

  }
  void Engine::changeResolution(int direction) {
    int newIndex = currentResIndex + direction;

    if (newIndex >= 0 && newIndex < 4) {
      currentResIndex = newIndex;
      Resolution target = resPool[currentResIndex];
      if (RenderServer::Get().changeResolution(target.width, target.height) < 0){
        std::cerr<< " failed to change resolution... uh oh!\n";
      }
      // tell OS
      std::cout<<"Resolution changed to:"<< target.width<<", "<<target.height<<"\n";
    }
  }


  void Engine::update(double dt) {
    for (auto *node : sceneNodes){

      node->update(dt);
    }
    // if (sceneNodes.size() > 1) {
    //   Node* player = sceneNodes[1];
    //
    //   float speed = 150.0f;
    //   player->position.x += speed * static_cast<float>(dt);
    // }
  }


  void Engine::run() {
    double accumulator = 0.0;
    const double FIXED_DT = 1.0 / 60.0; 
    const double MAX_DELTA = 0.25;
#ifdef PERFORMANCE
    double fpsTimer = 0.0;
    int frameCount = 0;
    uint64_t totalUpdateTicks = 0;
    uint64_t totalRenderTicks = 0;
#endif
    double elapsedTime = 0.0;
    uint64_t frequency = SDL_GetPerformanceFrequency();
    uint64_t currentTicks = SDL_GetPerformanceCounter();
    while (isRunning) {
      uint64_t newTicks = SDL_GetPerformanceCounter();

#ifdef PERFORMANCE
      double rawFrameTime = static_cast<double>(newTicks - currentTicks) / frequency;
#endif
      double frameTime = static_cast<double>(newTicks - currentTicks) / frequency;
      currentTicks = newTicks;

      if (frameTime > MAX_DELTA) {
        frameTime = MAX_DELTA;
      }
      elapsedTime += (frameTime * timeScale);
      accumulator += (frameTime * timeScale);

      handleEvents();
#ifdef PERFORMANCE
      uint64_t startUpdate = SDL_GetPerformanceCounter();
#endif
      while (accumulator >= FIXED_DT) {
        update(FIXED_DT);
        accumulator -= FIXED_DT;
      }
#ifdef PERFORMANCE
      uint64_t endUpdate = SDL_GetPerformanceCounter();
      totalUpdateTicks += (endUpdate - startUpdate);
      uint64_t startRender = SDL_GetPerformanceCounter();
#endif
      render(elapsedTime); // loads renderServer if needed
      RenderServer::Get().render(elapsedTime);
#ifdef DEBUG_WINDOW
      RenderServer::Get().DrawDebug();
#endif

#ifdef PERFORMANCE 
      uint64_t endRender = SDL_GetPerformanceCounter();
      totalRenderTicks += (endRender - startRender);
      frameCount++;
      fpsTimer += rawFrameTime;
      if (fpsTimer >= 1.0) {
        double avgFrameTimeMs = (fpsTimer / frameCount) * 1000.0;
        double currentFps = static_cast<double>(frameCount) / fpsTimer;

        double avgUpdateMs = (static_cast<double>(totalUpdateTicks) / frameCount / frequency) * 1000.0;
        double avgRenderMs = (static_cast<double>(totalRenderTicks) / frameCount / frequency) * 1000.0;

        term::print_engine_perf(currentFps, avgFrameTimeMs, avgUpdateMs, avgRenderMs);

        // Reset intervals
        frameCount = 0;
        fpsTimer = 0.0;
        totalUpdateTicks = 0;
        totalRenderTicks = 0;
      }
#endif
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
    RenderServer::Get().shutdown();
  }


  void Engine::render(double ft) {
    for (auto* node : sceneNodes) {
      if (node != nullptr) {
        node->triggerCompute();
        // node->triggerCompute(); // each node will share their rendering 
      }
    } 
  }
