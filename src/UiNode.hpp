#pragma once
#include "Node.hpp"
#include "RenderServer.hpp"
#include "TextureManager.hpp"
#include <string>

enum class UIAnchor {
  TopLeft,
  TopCenter,
  TopRight,
  CenterLeft,
  Center,
  CenterRight,
  BottomLeft,
  BottomCenter,
  BottomRight
};

enum class UIPivot {
  TopLeft,
  Center,
  BottomLeft
};

class UINode : public Node {
  public:
    GLuint textureID = 0;
    RenderItemLayer renderLayer = RenderItemLayer::UI_2D; 

    glm::vec2 size{100.0f, 100.0f};
    glm::vec2 offset{0.0f, 0.0f};

    UIAnchor anchor = UIAnchor::TopLeft;
    UIPivot pivot = UIPivot::TopLeft;

    UINode() = default;
    virtual ~UINode() = default;

    virtual void init(const std::string &texturePath, glm::vec2 sizePixels, glm::vec2 offsetPixels, UIAnchor uiAnchor = UIAnchor::TopLeft, UIPivot uiPivot = UIPivot::TopLeft) {
      this->textureID = TextureManager::Get().getTexture(texturePath);
      this->size = sizePixels;
      this->offset = offsetPixels;
      this->anchor = uiAnchor;
      this->pivot = uiPivot;
    }

 void printDebugInfo(int indent = 0) const {
      std::string pad(indent * 2, ' ');
      
      float localX = localTransform[3][0];
      float localY = localTransform[3][1];
      float globalX = globalTransform[3][0];
      float globalY = globalTransform[3][1];

      std::cout << pad << "UINODE: \n";
      std::cout << pad << "  Address:     " << this << "\n";
      std::cout << pad << "  Parent:      " << parent << (parent ? " (Has Parent)" : " (Root/None)") << "\n";
      std::cout << pad << "  Dirty :      " << (isDirty ? "TRUE" : "FALSE") << "\n";
      std::cout << pad << "  Texture ID:  " << textureID << "\n";
      std::cout << pad << "  Size:        [" << size.x << ", " << size.y << "]\n";
      std::cout << pad << "  Offset:      [" << offset.x << ", " << offset.y << "]\n";
      std::cout << pad << "  Local Pos:   [" << localX << ", " << localY << "]\n";
      std::cout << pad << "  Global Pos:  [" << globalX << ", " << globalY << "]\n";
      std::cout << pad << "  Children:    " << children.size() << "\n";

      for (auto* child : children) {
        if (auto* uiChild = dynamic_cast<UINode*>(child)) {
          uiChild->printDebugInfo(indent + 1);
        }
      }
    }
void computeTransforms(const glm::mat4& parentGlobal, bool parentIsDirty) override {
  bool needsRecalc = isDirty || parentIsDirty;

  if (needsRecalc) {
    glm::mat4 transform = glm::mat4(1.0f);

    glm::vec2 anchorPos = CalculateAnchorPosition();
    glm::vec2 pivotOffset = CalculatePivotOffset();
    glm::vec3 finalTranslation(anchorPos.x + offset.x + pivotOffset.x, anchorPos.y + offset.y + pivotOffset.y, 0.0f);

    transform = glm::translate(transform, finalTranslation);
    
    localTransform = transform;
    glm::vec3 parentTranslation = glm::vec3(parentGlobal[3]);
    glm::mat4 parentOnlyTranslation = glm::translate(glm::mat4(1.0f), parentTranslation);
    globalTransform = parentOnlyTranslation * localTransform;
    transform = glm::scale(transform, glm::vec3(size.x, size.y, 1.0f));
    globalTransform = glm::scale(globalTransform, glm::vec3(size.x, size.y, 1.0f));

    isDirty = false;
  }
  glm::mat4 childPassMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(globalTransform[3]));

  for (auto* child : children) {
    child->computeTransforms(childPassMatrix, needsRecalc);
  }
}

    void render(const glm::mat4 &parentTransform, bool parentIsDirty) override {
      computeTransforms(parentTransform, parentIsDirty);
      
      RenderInstance instance;
      instance.globalTransform = this->globalTransform;
      instance.textureID = this->textureID;
      instance.layer = this->renderLayer;

      instance.useCustomViewport = true;

      float screenW = RenderServer::Get().RSwidth;
      float screenH = RenderServer::Get().RSheight;
      // if (anchor == UIAnchor::TopLeft) {
      //   instance.vpX = static_cast<int>(offset.x);
      //   instance.vpY = static_cast<int>(screenH - size.y - offset.y);
      // }
      // else if (anchor == UIAnchor::TopCenter) {
      //   instance.vpX = static_cast<int>((screenW - size.x) * 0.5f + offset.x);
      //   instance.vpY = static_cast<int>(screenH - size.y - offset.y);
      // }
      // else if (anchor == UIAnchor::TopRight) {
      //   instance.vpX = static_cast<int>(screenW - size.x - offset.x);
      //   instance.vpY = static_cast<int>(screenH - size.y - offset.y);
      // }
      // else if (anchor == UIAnchor::CenterLeft) {
      //   instance.vpX = static_cast<int>(offset.x);
      //   instance.vpY = static_cast<int>((screenH - size.y) * 0.5f + offset.y);
      // }
      // else if (anchor == UIAnchor::Center) {
      //   instance.vpX = static_cast<int>((screenW - size.x) * 0.5f + offset.x);
      //   instance.vpY = static_cast<int>((screenH - size.y) * 0.5f + offset.y);
      // }
      // else if (anchor == UIAnchor::CenterRight) {
      //   instance.vpX = static_cast<int>(screenW - size.x - offset.x);
      //   instance.vpY = static_cast<int>((screenH - size.y) * 0.5f + offset.y);
      // }
      // else if (anchor == UIAnchor::BottomLeft) {
      //   instance.vpX = static_cast<int>(offset.x);
      //   instance.vpY = static_cast<int>(offset.y);
      // }
      // else if (anchor == UIAnchor::BottomCenter) {
      //   instance.vpX = static_cast<int>((screenW - size.x) * 0.5f + offset.x);
      //   instance.vpY = static_cast<int>(offset.y);
      // }
      // else if (anchor == UIAnchor::BottomRight) {
      //   instance.vpX = static_cast<int>(screenW - size.x - offset.x);
      //   instance.vpY = static_cast<int>(offset.y);
      // }
float globalX = this->globalTransform[3][0];
  float globalY = this->globalTransform[3][1];

  instance.vpX = static_cast<int>(globalX);

  // Since glViewport expects a bottom-left origin, but your ortho is top-left:
  screenH = RenderServer::Get().RSheight;
  instance.vpY = static_cast<int>(screenH - size.y - globalY);

  instance.vpWidth  = static_cast<int>(size.x);
  instance.vpHeight = static_cast<int>(size.y);
      instance.vpWidth  = static_cast<int>(size.x);
      instance.vpHeight = static_cast<int>(size.y);

      RenderServer::Get().SubmitInstance(instance);
      for (auto *elem : children) {
        elem->render(this->globalTransform, this->isDirty);
      }
      this->isDirty = false; 
    }

    bool Intersects(float mouseX, float mouseY) const {
      float globalX = globalTransform[3][0];
      float globalY = globalTransform[3][1];

      return (mouseX >= globalX && mouseX <= globalX + size.x &&
          mouseY >= globalY && mouseY <= globalY + size.y);
    }

  private:
    glm::vec2 CalculateAnchorPosition() {
      float parentWidth = RenderServer::Get().RSwidth;
      float parentHeight = RenderServer::Get().RSheight;

      if (parent != nullptr) {
        if (UINode* parentUI = dynamic_cast<UINode*>(parent)) {
          parentWidth = parentUI->size.x;
          parentHeight = parentUI->size.y;
        }
      }

      switch (anchor) {
        case UIAnchor::TopCenter:    return glm::vec2(parentWidth / 2.0f, 0.0f);
        case UIAnchor::TopRight:      return glm::vec2(parentWidth, 0.0f);
        case UIAnchor::CenterLeft:   return glm::vec2(0.0f, parentHeight / 2.0f);
        case UIAnchor::Center:       return glm::vec2(parentWidth / 2.0f, parentHeight / 2.0f);
        case UIAnchor::CenterRight:  return glm::vec2(parentWidth, parentHeight / 2.0f);
        case UIAnchor::BottomLeft:   return glm::vec2(0.0f, parentHeight);
        case UIAnchor::BottomCenter: return glm::vec2(parentWidth / 2.0f, parentHeight);
        case UIAnchor::BottomRight:  return glm::vec2(parentWidth, parentHeight);
        case UIAnchor::TopLeft:
        default:                     return glm::vec2(0.0f, 0.0f);
      }
    }

    glm::vec2 CalculatePivotOffset() {
      switch (pivot) {
        case UIPivot::Center:     return glm::vec2(-size.x / 2.0f, -size.y / 2.0f);
        case UIPivot::BottomLeft: return glm::vec2(0.0f, -size.y);
        case UIPivot::TopLeft:
        default:                  return glm::vec2(0.0f, 0.0f);
      }
    }
};
