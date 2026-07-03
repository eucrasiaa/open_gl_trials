#pragma once
#include "MeshNode.hpp"
#include "Engine.hpp"
#include <glm/trigonometric.hpp>
class AxisGuide : public MeshNode { 
      void update(double dt) override {
      this->rotation.x = glm::degrees(Engine::Get().yaw);    
      this->rotation.y = glm::degrees(Engine::Get().pitch);    
      this->rotation.z = glm::degrees(Engine::Get().roll);    
      isDirty=true;

      Node::update(dt);
    }

};
