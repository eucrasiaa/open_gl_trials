#pragma once
#include "RenderNode.hpp"
class PlayerElem : public RenderNode{
  public:
    float speed = 10.5f;

    glm::vec3 direction = glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)); 
    // void update(double dt){
    //   // SDL_Log("running update (player): %f",this->position.x);
    //     // this->position.x += 1.0;
    //     this->position += glm::vec3{-1.0f, 0.0f, 0.0f} * static_cast<float>(speed * dt);
    //     isDirty=true;
    //     Node::update(dt);
    //
    // }

    //really shoddy movement but mostly fighting with getting world scale LOL
    void update(double dt) {

      const float raw_max_x = 41.4f;
      const float raw_min_x = -41.4f;
      const float raw_max_y = 23.3f;
      const float raw_min_y = -23.3f;

      glm::vec2 actualSize = this->getWorldScale();
      float halfWidth  = actualSize.x * 0.5f;
      float halfHeight = actualSize.y * 0.5f;
      const float max_x = raw_max_x - halfWidth;
      const float min_x = raw_min_x + halfWidth;
      const float max_y = raw_max_y - halfHeight;
      const float min_y = raw_min_y + halfHeight;
      glm::vec3 currentPos(this->position.x, this->position.y, this->position.z);
      currentPos += this->direction * static_cast<float>(this->speed * dt);
      auto getRandomOffset = []() {
        return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 0.02f) - 0.01f;
      };
      if (currentPos.x <= min_x) {
        currentPos.x = min_x;
        this->direction.x = -this->direction.x;
        this->direction.y += getRandomOffset();
      } 
      else if (currentPos.x >= max_x) {
        currentPos.x = max_x;
        this->direction.x = -this->direction.x;
        this->direction.y += getRandomOffset();
      }
      if (currentPos.y <= min_y) {
        currentPos.y = min_y;
        this->direction.y = -this->direction.y;
        this->direction.x += getRandomOffset();
      } 
      else if (currentPos.y >= max_y) {
        currentPos.y = max_y;
        this->direction.y = -this->direction.y;
        this->direction.x += getRandomOffset();
      }
      this->direction = glm::normalize(this->direction);
      this->position.x = currentPos.x;
      this->position.y = currentPos.y;
      this->position.z = currentPos.z;
      this->isDirty = true;
      Node::update(dt);
    }
};
