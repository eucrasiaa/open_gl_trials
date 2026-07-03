#include "Engine.hpp"
#include "RenderServer.hpp"
#include <SDL_error.h>
#include <glad/glad.h>
#include "Defines.h"
bool Engine::init(const char* title, int width, int height) {
  if(!RenderServer::Get().init(title, width, height)){
  // if(!renderServer->init(title, width, height)){
    std::cerr<<"render server failed to initialize!\n";
    return false;
  }
  isRunning = true;
  return true;
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
          ShaderChange(-1); // Cycle down
          // changeResolution(-1); // Cycle down
          break;
        case SDLK_RIGHT:
          ShaderChange(1);  // Cycle up
          // changeResolution(1);  // Cycle up
          break;
        case SDLK_SPACE:
          inputState.ToggleMouse = !inputState.ToggleMouse;
          SDL_SetRelativeMouseMode(inputState.ToggleMouse? SDL_TRUE : SDL_FALSE);
          break;
      }
    }
    else if (inputState.ToggleMouse){ 
      if(event.type == SDL_MOUSEMOTION) {
        yaw   += event.motion.xrel * mouseSensitivity;
        pitch -= event.motion.yrel * mouseSensitivity; 
        // float maxPitch = glm::radians(-89.0f);
        // if (pitch > maxPitch)  pitch = maxPitch;
        // if (pitch < -maxPitch) pitch = -maxPitch;
        // if (pitch > 89.0f)  pitch = 89.0f;
        // if (pitch < -89.0f) pitch = -89.0f;
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

#ifdef CAMERA_DEBUG
      std::cout<<"Looking at: "<< glm::to_string(cameraLook)<<std::endl;
      std::cout<<"From at   : "<< glm::to_string(cameraPos)<<std::endl;
      
      std::cout<<"Up at   : "<< glm::to_string(cameraUp)<<std::endl;
      std::cout<<"Roll "<<roll<< "  Yaw: "<< yaw<<"  Pitch: "<<pitch<<std::endl;
      std::cout<<"________________________________\n"<<std::endl;

      term::move_up(6,true);
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

      // //
      // //
      // //
      // 
      // front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
      // front.y = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
      // front.z = sin(glm::radians(pitch));
      // front = glm::normalize(front);
      // // non tilted standard up
      // glm::vec3 standardUp = glm::vec3(0.0f, 1.0f, 0.0f);
      // glm::vec3 rolledUp = glm::rotate(glm::mat4(1.0f), glm::radians(roll), front) * glm::vec4(standardUp, 0.0f);
      // rolledUp = glm::normalize(rolledUp);
      //
      //
      // glm::vec3 right = glm::normalize(glm::cross(front, rolledUp));
      // // glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
      // const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
      // if (currentKeyStates[SDL_SCANCODE_W]) {
      //   cameraPos += front * movementSpeed;
      // }
      // if (currentKeyStates[SDL_SCANCODE_S]) {
      //   cameraPos -= front * movementSpeed;
      // }
      // if (currentKeyStates[SDL_SCANCODE_A]) {
      //   cameraPos -= right * movementSpeed; 
      // }
      // if (currentKeyStates[SDL_SCANCODE_D]) {
      //   cameraPos += right * movementSpeed; 
      // }
      // if (currentKeyStates[SDL_SCANCODE_Q]) {
      //   cameraPos.y += movementSpeed;
      // }
      //
      // if (currentKeyStates[SDL_SCANCODE_E]) {
      //   cameraPos.y -= movementSpeed; 
      // }
      // if (currentKeyStates[SDL_SCANCODE_Z]) {
      //   roll -= movementSpeed; 
      // }
      // if (currentKeyStates[SDL_SCANCODE_C]) {
      //   roll += movementSpeed; 
      // }
      //
      //
      // cameraLook = cameraPos + front; 
      // cameraUp   = glm::normalize(glm::cross(right, front));
      //
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

    RenderServer::Get().render(elapsedTime);
    #ifdef DEBUG_WINDOW
      RenderServer::Get().DrawDebug();
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
