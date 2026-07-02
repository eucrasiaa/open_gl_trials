
// #define RENDERSERVER_DEBUG

#include "MeshNode.hpp"
#include "PlayerElem.hpp"
#include "PlayerElem_alt.hpp"
#include "RenderItem.hpp"
#define SDL_MAIN_HANDLED
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "Engine.hpp"

#include "SpriteElement.hpp"

int main(int argc, char* argv[]) {
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
  // mySphere->scale = Vec3(50.0f, 50.0f,1.0f);
  

  
  Engine::Get().addNode(player2);
  Engine::Get().addNode(background);
  Engine::Get().addNode(myCube);
  Engine::Get().addNode(myTriangle);
  Engine::Get().addNode(mySphere);
    background->scale  = Vec3(16.0f, 9.0f, 1.0f);
    background->scale*=5.0f;
player2->scale     = Vec3(2.0f, 2.0f, 1.0f);
player->scale      = Vec3(4.0f, 4.0f, 4.0f);
myCube->scale      = Vec3(1.0f, 1.0f, 1.0f);
myTriangle->scale  = Vec3(1.0f, 1.0f, 1.0f);
mySphere->scale    = Vec3(1.0f, 1.0f, 1.0f);

myCube->scale    *=5.0f;
myTriangle->scale*=5.0f;
mySphere->scale  *=5.0f;

// for (auto& node : Engine::Get().sceneNodes) { 
  //   node->scale*=5;
  // }
  Engine::Get().run();
  Engine::Get().shutdown();
  return 0;
}
