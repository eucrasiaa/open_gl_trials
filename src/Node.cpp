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

void Node::triggerCompute(){ 
  // swap to a render call now
  render(glm::mat4(1.0f),  isDirty);
  // computeTransforms(glm::mat4(1.0f),  isDirty);
}


void Node::render(const glm::mat4 &parentTransform, bool parentIsDirty)  {
  computeTransforms(parentTransform, parentIsDirty);
  for (auto *elem : children) {
    elem->render(this->globalTransform, this->isDirty);
  }
  this->isDirty = false;
}

// void Node::computeTransforms(const glm::mat4& parentGlobal, bool parentIsDirty) {
//   bool needsRecalc = isDirty || parentIsDirty;
//
//   if (needsRecalc) {
//     //trs
//     localTransform = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0.0f));
//     localTransform = glm::rotate(localTransform, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
//     localTransform = glm::scale(localTransform, glm::vec3(scale.x, scale.y, 1.0f));
//
//     //global
//     globalTransform = parentGlobal * localTransform;
//     //fixflag
//     isDirty = false;
//   }
//
//   //tick down!
//   for (auto* child : children) {
//     child->computeTransforms(globalTransform, needsRecalc);
//   }
// }

void Node::computeTransforms(const glm::mat4& parentGlobal, bool parentIsDirty) {
  bool needsRecalc = isDirty || parentIsDirty;

  if (needsRecalc) {
    glm::mat4 transform = glm::mat4(1.0f);

    transform = glm::translate(transform, position);

    // rotate: apply y, x, z - as per euler 
    transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // YAW
    transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // PITCH
    transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // ROLL

    transform = glm::scale(transform, scale);

    localTransform = transform;
    globalTransform = parentGlobal * localTransform;

    isDirty = false;
  }
  //tick down
  for (auto* child : children) {
    child->computeTransforms(globalTransform, needsRecalc);
  }
}


// void Node::render(const glm::mat4 &parentTransform = glm::mat4(1.0f)) {
//   glm::mat4 globalTransform = parentTransform * calculateTransform();
//
//   for (auto *elem : children) {
//     elem->render(globalTransform);
//   }
// }

void Node::addChild(Node *node) {
  if (node != nullptr) {
    children.push_back(node);
    node->parent=this;
  }
}

// void Node::computeTransforms() {
//   // ident matrix
//   glm::mat4 transform = glm::mat4(1.0f);
//   // loc
//   transform =
//     glm::translate(transform, glm::vec3(position.x, position.y, 0.0f));
//   // rot
//   transform = glm::rotate(transform, glm::radians(rotation),
//       glm::vec3(0.0f, 0.0f, 1.0f));
//   // scale
//   transform = glm::scale(transform, glm::vec3(scale.x, scale.y, 1.0f));
//
//   return transform;
// }

