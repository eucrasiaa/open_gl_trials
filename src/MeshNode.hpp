#pragma once
#include "Node.hpp"
#include "MeshManager.hpp"
#include "RenderNode.hpp"
#include "glad/glad.h"
#include "RenderServer.hpp"

class MeshNode : public Node {
public:
    GLuint textureID = 0;
    GLuint vaoID = 0;
    GLuint indexCount = 0;
    RenderItemLayer renderLayer = RenderItemLayer::OPAQUE;

    void init(const std::string& objPath, const std::string& texturePath, RenderItemLayer layer) {
        this->textureID = TextureManager::Get().getTexture(texturePath);
        this->renderLayer = layer;

        GLuint instanceVBO = RenderServer::Get().gInstanceVBO; 
        MeshData mesh = MeshManager::Get().loadMesh(objPath, instanceVBO);
        
        this->vaoID = mesh.vaoID;
        this->indexCount = mesh.indexCount;
    }
    void update(double dt) override {
#ifdef NODE_DEBUF
      std::cout<<"MeshUpdateTick :"<< vaoID<<std::endl;
#endif
      this->rotation.x += 45.0f * dt;    
      this->rotation.y += 45.0f * dt;
      // this->rotation.z += 45.0f * dt;
            isDirty=true;

      Node::update(dt);
    }
    void render(const glm::mat4 &parentTransform, bool parentIsDirty) override {
        computeTransforms(parentTransform, parentIsDirty);

        RenderInstance instance;
        instance.globalTransform = this->globalTransform;
        instance.textureID = this->textureID;
        
        instance.vaoID = this->vaoID;
        instance.indexCount = this->indexCount;
        instance.layer = this->renderLayer;

        RenderServer::Get().SubmitInstance(instance);

        for (auto *elem : children) {
            elem->render(this->globalTransform, this->isDirty);
        }
        
        this->isDirty = false; 
    }
};
