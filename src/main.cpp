

#if defined(_WIN32)
  // #define NOMINMAX
  // #define WIN32_LEAN_AND_MEAN
  // #include <windows.h>
  #include <libloaderapi.h>
  // #undef OPAQUE 
  // #undef TRANSPARENT
#elif defined(__linux__)
  #include <unistd.h>
  #include <limits.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h> 
#endif

#include "MeshNode.hpp"
#include "PlayerElem.hpp"
#include "PlayerElem_alt.hpp"
#include "RenderItem.hpp"
#include <fstream>
#define SDL_MAIN_HANDLED
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "Engine.hpp"

#include "UiNode.hpp"
#include <filesystem>
#include "AxisGuide.hpp"
#include "wtypes/Vec2.hpp"
#include "SpriteElement.hpp"

static std::filesystem::path EXECUTABLE_DIR;

int main(int argc, char* argv[]) {
  //prelim handling stuff 
  

#if defined(_WIN32)
  wchar_t buffer[MAX_PATH];
  DWORD length = GetModuleFileNameW(NULL, buffer, MAX_PATH);
  if (length == 0) {
      throw std::runtime_error("Windows: Failed to retrieve executable path.");
  }
  std::wcout<<"Windows: "<<buffer<<std::endl;
  EXECUTABLE_DIR = std::filesystem::path(buffer).parent_path();
#elif defined(__linux__)
  char buffer[PATH_MAX];
  ssize_t length = readlink("/proc/self/exe", buffer, PATH_MAX);
  if (length == -1) {
    throw std::runtime_error("Linux: Failed to read executable symlink.");
  }
  EXECUTABLE_DIR = std::filesystem::path(std::string(buffer)).parent_path();
#elif defined(__APPLE__)
  uint32_t size = 0;
  _NSGetExecutablePath(nullptr, &size); 
  char buffer[size];
  if (_NSGetExecutablePath(buffer, &size) != 0) {
      throw std::runtime_error("macOS: Failed to retrieve executable path.");
  }
  EXECUTABLE_DIR = std::filesystem::weakly_canonical(std::filesystem::path(buffer)).parent_path();
#endif 

  SDL_SetMainReady();
  if (!Engine::Get().init("OpenGL Engine Layout", 1280, 720)) {
    return 1;
  }
  std::vector<Node*> allNodes;
  RenderNode* background = new RenderNode();
  background->init("../assets/textures/pleasentgradient.png", RenderItemLayer::OPAQUE);
  // background->position = Vec3(640.0f, 360.0f); 
  // background->scale = Vec3(1280.0f, 720.0f);
  background->position = Vec3(0.0f, 0.0f, -1.0f); 
  background->scale    = Vec3(16.0f, 9.0f, 1.0f); 



  PlayerElem* player = new PlayerElem();
  player->init("../assets/textures/player.png", RenderItemLayer::TRANSPARENT);
  // player->position = Vec2(1280.0/2.0, 720.0/2.0);

  player->position = Vec3(0.0, 0.0,0.0f);
  player->scale = Vec3(1.0f, 1.0f,1.0f);

  PlayerElem_alt* player2 = new PlayerElem_alt();
  player2->init("../assets/textures/x.png", RenderItemLayer::TRANSPARENT);
  // player2->position = Vec3(1280.0/2.0, 720.0/2.0);
  player2->position = Vec3(0.0f,0.0f,0.0f);
  player2->scale = Vec3(1.0f,1.0f,1.0f);
  // player2->scale = Vec3(50.0f, 50.0f,1.0f);
  player2->addChild(player);


  MeshNode* myCube = new MeshNode();
  myCube->init("../assets/meshes/MiniCube.obj", "../assets/textures/X.png", RenderItemLayer::OPAQUE);
  // myCube->position = Vec3(640.0f, 360.0f,0.0f); 
  myCube->position = Vec3(0.0f,-10.0f,4.0f);
  myCube->scale = Vec3(1.0f, 1.0f, 1.0f);
  // myCube->scale = Vec3(50.0f, 50.0f);


  MeshNode* myTriangle = new MeshNode();
  myTriangle->init("../assets/meshes/MiniTriangle.obj", "../assets/textures/X.png", RenderItemLayer::OPAQUE);

  myTriangle->position = Vec3(30.0f,0.0f,0.0f);
  // myTriangle->position = Vec3((640.0f*1.5f), 360.0f,0.0f); 

  myTriangle->scale = Vec3(1.0f, 1.0f,1.0f);
  // myTriangle->scale = Vec3(50.0f, 50.0f,1.0f);
  MeshNode* mySphere = new MeshNode();
  mySphere->init("../assets/meshes/MiniSphere.obj", "../assets/textures/X.png", RenderItemLayer::OPAQUE);
  // mySphere->position = Vec3(640.0f/2, 360.0f/2,0.0f); 

  mySphere->position = Vec3(-30.5f,0.0f,0.0f);
  mySphere->scale = Vec3(1.0f, 1.0f,1.0f);


  AxisGuide* UI_ELEM_2d = new AxisGuide();
  UI_ELEM_2d->init("../assets/meshes/Axis_v2.obj", "../assets/textures/Axis_guide.png", Vec2(500.0f,500.0f),Vec2(0.0f,0.0f), UIAnchor::TopRight,UIPivot::Center);
  Engine::Get().addNode(UI_ELEM_2d);
  // devNode = UI_ELEM_2d;
  // UI_ELEM_2d->position.z-=5.0f;
  UI_ELEM_2d->scale*=0.05;

  //
  // UINode* AA = new UINode();
  // AA->init("../assets/textures/Lain.jpg", Vec2(500.0f,500.0f),Vec2(10.0f,10.0f), UIAnchor::BottomLeft,UIPivot::Center);
  // Engine::Get().addNode(AA);
  // AA->scale*=10.0f;
  // UINode* BB = new UINode();
  // BB->init("../assets/textures/Lain.jpg", Vec2(500.0f,500.0f),Vec2(10.0f,10.0f), UIAnchor::BottomLeft,UIPivot::Center);
  // Engine::Get().addNode(BB);
  // AA->addChild(BB);
//
//   AxisGuide* UI_ELEM_2d = new AxisGuide();
//   UI_ELEM_2d->init("../assets/meshes/Comp2.obj", "../assets/textures/Compass.png", Vec2(400.0f,400.0f),Vec2(10.0f,10.0f), UIAnchor::TopCenter,UIPivot::Center);
//   Engine::Get().addNode(UI_ELEM_2d);
//   UI_ELEM_2d->scale*=0.5;
// UI_ELEM_2d->rotation.z=180;
//   UI_ELEM_2d->position.z+=0.1f;
// UI_ELEM_2d->rotation.y=20;

  // mySphere->scale = Vec3(50.0f, 50.0f,1.0f);
  //
  // MeshNode* Axes2 = new MeshNode();
  // Axes2->init("../assets/meshes/Axis_v2.obj", "../assets/textures/Axis_guide.png", RenderItemLayer::OPAQUE);
  //
  // Axes2->position = Vec3(0.0f,0.0f,-20.0f);
  //
  // Axes2->scale = Vec3(1.0f, 1.0f, 1.0f);
  // Engine::Get().addNode(Axes2);
  // AxisGuide* Axis_guide = new AxisGuide();
  // Axis_guide->init("../assets/meshes/Axis_guide.obj", "../assets/textures/Axis_guide.png", RenderItemLayer::UI_3D);
  // Axis_guide->position = Vec3(0.0f,0.0f,0.0f);
  // Axis_guide->scale = Vec3(1.0f, 1.0f, 1.0f);
  // Axis_guide->scale *=0.7;
  // Engine::Get().addNode(Axis_guide);
  //

  // UINode* UI_ELEM_2d = new UINode();
  // UI_ELEM_2d->init("../assets/textures/X.png", Vec2(100.0f,100.0f),Vec2(10.0f,10.0f), UIAnchor::TopRight,UIPivot::Center);
  // Engine::Get().addNode(UI_ELEM_2d);
  // MeshNode* wolf = new MeshNode();
  // wolf->init("../assets/meshes/_/_wolf.obj", "../assets/textures/_/wolf.png", RenderItemLayer::OPAQUE);
  // wolf->position = Vec3(-30.5f,40.0f,0.0f);
  // wolf->scale = Vec3(1.0f, 1.0f,1.0f);
  // wolf->scale *=15.0f;
  //Engine::Get().addNode(wolf);
  //
  MeshNode* Horse = new MeshNode();
  Horse->init("../assets/meshes/_/_horse.obj", "../assets/textures/_/_horse.jpg", RenderItemLayer::OPAQUE);
  Horse->position = Vec3(30.5f,40.0f,0.0f);
  Horse->scale = Vec3(1.0f, 1.0f,1.0f);
  Horse->scale*=0.01f;
  Engine::Get().addNode(Horse);
  //
  //
  // MeshNode* Fish = new MeshNode();
  // Fish->init("../assets/meshes/Fish.obj", "../assets/textures/Fish.png", RenderItemLayer::OPAQUE);
  // Fish->position = Vec3(0.5f,40.0f,0.0f);
  // Fish->scale = Vec3(1.0f, 1.0f,1.0f);
  // Fish->scale*=4.0f;
  // Engine::Get().addNode(Fish);

  Engine::Get().addNode(player2);
  Engine::Get().addNode(background);
  Engine::Get().addNode(myCube);
  Engine::Get().addNode(myTriangle);
  Engine::Get().addNode(mySphere);
  background->scale  = Vec3(16.0f, 9.0f, 1.0f);
  background->scale*=2.0f;
  player2->scale     = Vec3(2.0f, 2.0f, 1.0f);
  player->scale      = Vec3(4.0f, 4.0f, 4.0f);
  myCube->scale      = Vec3(1.0f, 1.0f, 1.0f);
  myTriangle->scale  = Vec3(1.0f, 1.0f, 1.0f);
  mySphere->scale    = Vec3(1.0f, 1.0f, 1.0f);

  myCube->scale    *=5.0f;
  myTriangle->scale*=5.0f;
  mySphere->scale  *=5.0f;
  player->scale *= 1.0f;

  // for (auto& node : Engine::Get().sceneNodes) { 
  //   node->scale*=5;
  // }
  Engine::Get().run();
  Engine::Get().shutdown();
  return 0;
}
