#pragma once
#include "RenderNode.hpp"

class PlayerElem : public RenderNode{
  public:
    float speed = 0.5f;
  void update(double dt){
    // SDL_Log("running update (player): %f",this->position.x);
      // this->position.x += 1.0;
      this->position += Vec2{-1.0,0.0} * speed * dt; 
      Node::update(dt);

  }

  
};
