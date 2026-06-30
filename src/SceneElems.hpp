#pragma once
#include "./wtypes/Vec2.hpp"
#include "RenderItem.hpp"
#include "RenderServer.hpp"
#include "TextureManager.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

class Node {
public:
  Vec2 position = Vec2::ZERO;
  float rotation = 0.0f; // x/y tracking or angle mapping
  Vec2 scale = Vec2(1.0f, 1.0f);

  std::vector<Node *> children;

  virtual ~Node() {
    for (auto *elem : children) {
      delete elem;
    }
  }

  virtual void update(double dt) {
    for (auto *elem : children) {
      elem->update(dt);
    }
  }

  virtual void render(const glm::mat4 &parentTransform = glm::mat4(1.0f)) {
    glm::mat4 globalTransform = parentTransform * calculateTransform();

    for (auto *elem : children) {
      elem->render(globalTransform);
    }
  }

  virtual void addChild(Node *node) {
      if (node != nullptr) {
        children.push_back(node);
      }
  }
  // virtual void render(const glm::mat4& parentTransform = glm::mat4(1.0f)) {
  //    glm::mat4 globalTransform = parentTransform * calculateTransform();
  //    for (auto *elem : children) {
  //      elem->render(globalTransform);
  //    }
  //  }

  glm::mat4 calculateTransform() {
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
};

class RenderElem : public Node {
public:
  std::string texture = "";
  GLuint renderID;
  RenderItemLayer renderLayer;
  RenderElem() = default;
  void init(const std::string &texturePath, RenderItemLayer renderLayer) {
    GLuint texID = TextureManager::Get().getTexture(texturePath);

    std::vector<Vertex> localQuad = {
        {{-0.5f, -0.5f, 0.0f},
         {1.0f, 1.0f, 1.0f, 1.0f},
         {0.0f, 0.0f},
         0.0f}, // Bottom-Left
        {{0.5f, -0.5f, 0.0f},
         {1.0f, 1.0f, 1.0f, 1.0f},
         {1.0f, 0.0f},
         0.0f}, // Bottom-Right
        {{0.5f, 0.5f, 0.0f},
         {1.0f, 1.0f, 1.0f, 1.0f},
         {1.0f, 1.0f},
         0.0f}, // Top-Right
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, 0.0f}
        // Top-Left
    };

    std::vector<GLuint> indices = {0, 1, 2, 2, 3, 0};

    this->renderID = RenderServer::Get().RegisterItem(localQuad, indices, texID,
                                                      "Basic", renderLayer);
  }

  void render(const glm::mat4 &parentTransform = glm::mat4(1.0f)) override {

    glm::mat4 globalTransform = parentTransform * calculateTransform();

    RenderServer::Get().UpdateTransform(this->renderID, globalTransform);

    for (auto *elem : children) {
      elem->render(globalTransform);
    }
  }
  //   void update(double dt) override{
  //   SDL_Log("running update (renderElem): %f",this->position.x);
  //     // this->position.x += 1.0;
  //     // this->position += Vec2{1.0,0.0} * 0.5f * dt;
  //     Node::update(dt);
  //
  // }
};
