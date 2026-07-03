#pragma once
#include "glad/glad.h"
#include <unordered_map>
#include <string>

class TextureManager{
  private:
    GLuint linearSampler=0;
    GLuint nearestSampler=0;
    struct TextureResource {
      GLuint id;
      GLuint64 handleLinear;
      GLuint64 handleNearest;
      unsigned int refCount;
      int width;
      int height;
      int channels;
    };
    std::unordered_map<std::string, TextureResource> textureCache;

    TextureManager() = default;
    ~TextureManager() = default;
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

  public:
    void init(GLuint lS, GLuint nS){
      linearSampler=lS;
      nearestSampler=nS;
      return;
    }
    static TextureManager& Get() {
      static TextureManager instance;
      return instance;
    }

    GLuint getTexture(const std::string& filePath);
    void Release(const std::string& filePath);
    
};
