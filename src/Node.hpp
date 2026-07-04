#pragma once
#include "./wtypes/Vec3.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "Defines.h"

class Node {
public:
  // Vec2 position = Vec2::ZERO;
  // float rotation = 0.0f; // x/y tracking or angle mapping
  // Vec2 scale = Vec2(1.0f, 1.0f);
  glm::vec3 position = glm::vec3(0.0f);
  glm::vec3 rotation = glm::vec3(0.0f); // x (pitch), y (yaw), z (roll) in degrees
  glm::vec3 scale    = glm::vec3(1.0f);
  glm::mat4 localTransform = glm::mat4(1.0f);
  glm::mat4 globalTransform = glm::mat4(1.0f);
  bool isDirty = true; // for initial sync math!  
  std::vector<Node *> children;

  Node* parent=nullptr;

  glm::vec2 getWorldScale() const {
    float worldWidth  = glm::length(glm::vec3(globalTransform[0]));
    float worldHeight = glm::length(glm::vec3(globalTransform[1]));
    return glm::vec2(worldWidth, worldHeight);
}
  void markDirty() {
    isDirty = true;
  }
  virtual ~Node();
  virtual void update(double dt);
  // virtual void render(const glm::mat4 &parentTransform);
  virtual void addChild(Node *node);
  
  virtual void render(const glm::mat4& parentGlobal, bool parentIsDirty);
  void triggerCompute();
  virtual void computeTransforms(const glm::mat4& parentGlobal, bool parentIsDirty);


  };
