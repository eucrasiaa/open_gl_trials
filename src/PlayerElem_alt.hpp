#pragma once
#include "RenderNode.hpp"

class PlayerElem_alt : public RenderNode{
  public:
    float speed = 10.0f;
  void update(double dt){
    // SDL_Log("running update (player_alt): %f",this->position.y);
      // this->position.x += 1.0;
      // this->position += glm::vec3{0.0f, -1.0f, 0.0f} * static_cast<float>(speed * dt);
      // isDirty=true;
      Node::update(dt);

  }
  

  
  
};
