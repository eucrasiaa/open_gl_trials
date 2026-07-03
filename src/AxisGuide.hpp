#pragma once
#include "MeshNode.hpp"
#include "Engine.hpp"
#include "UI3DMeshNode.hpp"
#include <glm/trigonometric.hpp>
class AxisGuide : public UI3DMeshNode { 
      void update(double dt) override {
      // this->rotation.x = glm::degrees(Engine::Get().pitch);    
      // this->rotation.y = glm::degrees(Engine::Get().yaw);    
      // this->rotation.z = glm::degrees(Engine::Get().roll);    
      // isDirty=true;
      //
      Node::update(dt);
    }

};
