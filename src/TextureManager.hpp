#pragma once
#include "glad/glad.h"
#include <unordered_map>
#include <string>

class TextureManager{
  private:
  struct TextureResource {
    GLuint id;
    unsigned int refCount;
    int width;
    int height;
    int channels;
  };
  std::unordered_map<std::string, TextureResource> textureCache;
  
  public:

    GLuint GetTexture(const std::string& filePath);
    void Release(const std::string& filePath);
};
