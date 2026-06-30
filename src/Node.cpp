#include "Node.hpp"


#pragma once
#include "./wtypes/Vec2.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

Node::~Node() {
  for (auto *elem : children) {
    delete elem;
  }
}

void Node::update(double dt) {
  for (auto *elem : children) {
    elem->update(dt);
  }
}

void Node::render(const glm::mat4 &parentTransform = glm::mat4(1.0f)) {
  glm::mat4 globalTransform = parentTransform * calculateTransform();

  for (auto *elem : children) {
    elem->render(globalTransform);
  }
}

void Node::addChild(Node *node) {
  if (node != nullptr) {
    children.push_back(node);
  }
}

glm::mat4 Node::calculateTransform() {
  // ident matrix
  glm::mat4 transform = glm::mat4(1.0f);
  // loc
  transform =
    glm::translate(transform, glm::vec3(position.x, position.y, 0.0f));
  // rot
  transform = glm::rotate(transform, glm::radians(rotation),
      glm::vec3(0.0f, 0.0f, 1.0f));
  // scale
  transform = glm::scale(transform, glm::vec3(scale.x, scale.y, 1.0f));

  return transform;
}

