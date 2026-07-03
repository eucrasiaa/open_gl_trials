#pragma once
#include "MeshNode.hpp"
#include "UiNode.hpp"

class UI3DMeshNode : public MeshNode {
  public:
    glm::vec2 uiSizePixels{150.0f, 150.0f};
    glm::vec2 uiOffsetPixels{20.0f, 20.0f};
    UIAnchor anchor = UIAnchor::TopRight;
    UIPivot pivot = UIPivot::TopLeft;
    UI3DMeshNode() {
      this->renderLayer = RenderItemLayer::UI_3D; 
    }
    using MeshNode::init;


    virtual void init(const std::string& objPath, const std::string& texturePath,  
                      glm::vec2 sizePixels, glm::vec2 offsetPixels, 
                      UIAnchor uiAnchor = UIAnchor::TopLeft, UIPivot uiPivot = UIPivot::TopLeft) {
        this->textureID = TextureManager::Get().getTexture(texturePath);
        this->uiSizePixels = sizePixels;
        this->uiOffsetPixels = offsetPixels;
        this->anchor = uiAnchor;
        this->pivot = uiPivot;

        GLuint instanceVBO = RenderServer::Get().gInstanceVBO; 
        MeshData mesh = MeshManager::Get().loadMesh(objPath, instanceVBO);
        
        this->vaoID = mesh.vaoID;
        this->indexCount = mesh.indexCount;
    }
    void render(const glm::mat4 &parentTransform, bool parentIsDirty) override {
      computeTransforms(parentTransform, parentIsDirty);

      RenderInstance instance;
      instance.globalTransform = this->globalTransform;
      instance.textureID       = this->textureID;
      instance.vaoID           = this->vaoID;
      instance.indexCount      = this->indexCount;
      instance.layer           = this->renderLayer;
      instance.useCustomViewport = true;

      float screenW = RenderServer::Get().RSwidth;
      float screenH = RenderServer::Get().RSheight;

      if (anchor == UIAnchor::TopLeft) {
        instance.vpX = static_cast<int>(uiOffsetPixels.x);
        instance.vpY = static_cast<int>(screenH - uiSizePixels.y - uiOffsetPixels.y);
      }
      else if (anchor == UIAnchor::TopCenter) {
        instance.vpX = static_cast<int>((screenW - uiSizePixels.x) * 0.5f + uiOffsetPixels.x);
        instance.vpY = static_cast<int>(screenH - uiSizePixels.y - uiOffsetPixels.y);
      }
      else if (anchor == UIAnchor::TopRight) {
        instance.vpX = static_cast<int>(screenW - uiSizePixels.x - uiOffsetPixels.x);
        instance.vpY = static_cast<int>(screenH - uiSizePixels.y - uiOffsetPixels.y);
      }
      else if (anchor == UIAnchor::CenterLeft) {
        instance.vpX = static_cast<int>(uiOffsetPixels.x);
        instance.vpY = static_cast<int>((screenH - uiSizePixels.y) * 0.5f + uiOffsetPixels.y);
      }
      else if (anchor == UIAnchor::Center) {
        instance.vpX = static_cast<int>((screenW - uiSizePixels.x) * 0.5f + uiOffsetPixels.x);
        instance.vpY = static_cast<int>((screenH - uiSizePixels.y) * 0.5f + uiOffsetPixels.y);
      }
      else if (anchor == UIAnchor::CenterRight) {
        instance.vpX = static_cast<int>(screenW - uiSizePixels.x - uiOffsetPixels.x);
        instance.vpY = static_cast<int>((screenH - uiSizePixels.y) * 0.5f + uiOffsetPixels.y);
      }
      else if (anchor == UIAnchor::BottomLeft) {
        instance.vpX = static_cast<int>(uiOffsetPixels.x);
        instance.vpY = static_cast<int>(uiOffsetPixels.y);
      }
      else if (anchor == UIAnchor::BottomCenter) {
        instance.vpX = static_cast<int>((screenW - uiSizePixels.x) * 0.5f + uiOffsetPixels.x);
        instance.vpY = static_cast<int>(uiOffsetPixels.y);
      }
      else if (anchor == UIAnchor::BottomRight) {
        instance.vpX = static_cast<int>(screenW - uiSizePixels.x - uiOffsetPixels.x);
        instance.vpY = static_cast<int>(uiOffsetPixels.y);
      }

      instance.vpWidth  = static_cast<int>(uiSizePixels.x);
      instance.vpHeight = static_cast<int>(uiSizePixels.y);
      RenderServer::Get().SubmitInstance(instance);

      for (auto *elem : children) {
        elem->render(this->globalTransform, this->isDirty);
      }

      this->isDirty = false; 
    }
};
