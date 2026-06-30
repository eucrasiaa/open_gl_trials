#pragma once
#include "Node.hpp"

#include "RenderItem.hpp"
#include "RenderServer.hpp"
#include "TextureManager.hpp"


class RenderNode : public Node {
public:
  std::string texture = "";
  GLuint renderID;
  RenderItemLayer renderLayer;
  RenderNode() = default;
  virtual void init(const std::string &texturePath, RenderItemLayer renderLayer) {
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
                                                      "MVP", renderLayer);

    SDL_Log("init on render!");
    std::cout<<typeid(this).name() << std::endl;
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
