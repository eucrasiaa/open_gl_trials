#pragma once
#include "RenderNode.hpp"

class PlayerElem_alt : public RenderNode{
  public:
    float speed = 10.0f;
  void update(double dt){
    // SDL_Log("running update (player_alt): %f",this->position.y);
      // this->position.x += 1.0;
      this->position += Vec2{0.0,-1.0} * speed * dt; 
      isDirty=true;
      Node::update(dt);

  }
  

  
  
};
