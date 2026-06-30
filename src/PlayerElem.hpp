#pragma once

#include "SceneElems.hpp"
class PlayerElem : public Node{
  public:
    float speed = 100.0f;
  void update(double dt){
      this->position += Vec2{-1.0,0.0} * speed * dt; 
      Node::update(dt);

  }

  
};
