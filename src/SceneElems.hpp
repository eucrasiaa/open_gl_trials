#pragma once
#include "./wtypes/Vec2.hpp"
#include <vector>
#include "RenderItem.hpp"
#include "TextureManager.hpp"
#include "RenderServer.hpp"
#include <glm/gtc/matrix_transform.hpp>

class Node {
  public:
    Vec2 position = Vec2::ZERO;
    float rotation = 0.0f; // x/y tracking or angle mapping
    Vec2 scale    = Vec2(1.0f, 1.0f);

    std::vector<Node*> children;

    ~Node() {
      for (auto* elem : children) {
        delete elem;
      }
    }

    void render();

    glm::mat4 calculateTransform() {
      //ident matrix
      glm::mat4 transform = glm::mat4(1.0f);
      //loc
      transform = glm::translate(transform, glm::vec3(position.x, position.y, 0.0f));
      //rot
      transform = glm::rotate(transform, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
      //scale
      transform = glm::scale(transform, glm::vec3(scale.x, scale.y, 1.0f));

      return transform;
    }
};



class RenderElem : public Node {
  public:
    std::string texture ="";
    GLuint renderID;
    RenderElem(const std::string & txt): texture(txt){
      std::cout<<"hi initializied with texture" << txt<<std::endl;
      RenderItem item;
      item.textureID = TextureManager::Get().getTexture("player.png");
      item.modelMatrix = this->calculateTransform();
      item.layer = RenderItemLayer::OPAQUE;
      this->renderID = RenderServer::Get().registerItem(item);

    }
};
