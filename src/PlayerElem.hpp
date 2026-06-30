#pragma once
#include <SDL2/sdl.h>
#include "SceneElems.hpp"
class PlayerElem : public RenderElem{
  public:
    float speed = 10.0f;
  void update(double dt){
    SDL_Log("running update (player): %f",this->position.x);
      // this->position.x += 1.0;
      this->position += Vec2{-1.0,0.0} * speed * dt; 
      Node::update(dt);

  }

  
};
