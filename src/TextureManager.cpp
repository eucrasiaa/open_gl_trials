#include "TextureManager.hpp"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

void TextureManager::Release(const std::string& filePath) {
  auto it = textureCache.find(filePath); 
  if (it != textureCache.end()) {
    it->second.refCount--;

    if (it->second.refCount == 0) {
      glDeleteTextures(1, &it->second.id);
      textureCache.erase(it);
    }
  }
}

GLuint TextureManager::getTexture(const std::string& filePath) {   
  auto it = textureCache.find(filePath); 
  if (it != textureCache.end()) {
    it->second.refCount++; 
    return it->second.id;
  }
  else {
    int localWidth = 0;
    int localHeight = 0;
    int localChannels = 0;
    unsigned char* imageData = stbi_load(filePath.c_str(), &localWidth, &localHeight, &localChannels, 0);
    //safety!! 
    if (!imageData) {
      std::cerr<<"failed to load image, name was: "<<filePath<<"\n";
      return 0; 
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLenum internalFormat = GL_RGB;
    GLenum dataFormat = GL_RGB;
    if (localChannels == 1) { internalFormat = GL_R8; dataFormat = GL_RED; }
    else if (localChannels == 3) { internalFormat = GL_RGB8; dataFormat = GL_RGB; }
    else if (localChannels == 4) { internalFormat = GL_RGBA8; dataFormat = GL_RGBA; }
    //??
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, localWidth, localHeight, 0, dataFormat, GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(GL_TEXTURE_2D);
    //configs?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    stbi_image_free(imageData);
    glBindTexture(GL_TEXTURE_2D, 0);
    TextureResource newResource;
    newResource.id = texture;
    newResource.refCount = 1;
    newResource.width = localWidth;
    newResource.height = localHeight;
    newResource.channels = localChannels;

    textureCache[filePath] = newResource;

    return texture;
  }
}
