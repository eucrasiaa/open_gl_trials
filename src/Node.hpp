#pragma once
#include "./wtypes/Vec2.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Node {
public:
  Vec2 position = Vec2::ZERO;
  float rotation = 0.0f; // x/y tracking or angle mapping
  Vec2 scale = Vec2(1.0f, 1.0f);
  glm::mat4 localTransform = glm::mat4(1.0f);
  glm::mat4 globalTransform = glm::mat4(1.0f);
  bool isDirty = true; // for initial sync math!  
  std::vector<Node *> children;

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
