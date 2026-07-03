#include "TextureManager.hpp"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "Defines.h"
void TextureManager::Release(const std::string& filePath) {
  auto it = textureCache.find(filePath); 
  if (it != textureCache.end()) {
    it->second.refCount--;

    if (it->second.refCount == 0) {
      // RELEASE HANDLES FIRST
      glMakeTextureHandleNonResidentARB(it->second.handleLinear);
      glMakeTextureHandleNonResidentARB(it->second.handleNearest);
      glDeleteTextures(1, &it->second.id);
      textureCache.erase(it);
    }
  }
}

MaterialHandles TextureManager::getTexture(const std::string& filePath) {   
#ifdef TEXTUREMGR_DEBUG 
  std::cout<<"attempted to load "<<filePath<<std::endl;
#endif
  auto it = textureCache.find(filePath); 
  if (it != textureCache.end()) {

    #ifdef TEXTUREMGR_DEBUG 
      std::cout<<"  Already Found! "<<filePath<<std::endl;
    #endif
    it->second.refCount++; 
    return MaterialHandles{it->second.handleLinear,it->second.handleNearest};
  }
  else {

    #ifdef TEXTUREMGR_DEBUG 
      std::cout<<"  New Texture loading start: "<<filePath<<std::endl;
    #endif
    int localWidth = 0;
    int localHeight = 0;
    int localChannels = 0;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* imageData = stbi_load(filePath.c_str(), &localWidth, &localHeight, &localChannels, 0);
    //safety!! 
    if (!imageData) {
      std::cerr<<"failed to load image, name was: "<<filePath<<"\n";
      return {0,0}; 
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
    // glGenerateMipmap(GL_TEXTURE_2D);
    //configs?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // bindless texture attempt
    GLuint64 handleLinear  = glGetTextureSamplerHandleARB(texture, linearSampler);
    GLuint64 handleNearest = glGetTextureSamplerHandleARB(texture, nearestSampler);

    glMakeTextureHandleResidentARB(handleLinear);
    glMakeTextureHandleResidentARB(handleNearest);

    stbi_image_free(imageData);
    glBindTexture(GL_TEXTURE_2D, 0);
    TextureResource newResource;
    newResource.id = texture;
    newResource.handleLinear = handleLinear;
    newResource.handleNearest = handleNearest;
    newResource.refCount = 1;
    newResource.width = localWidth;
    newResource.height = localHeight;
    newResource.channels = localChannels;
        #ifdef TEXTUREMGR_DEBUG 
    std::cout<<"loaded texture: "<<texture<<" "<<localWidth<<" "<<localHeight<<" "<<localChannels<<std::endl;
#endif
    textureCache[filePath] = newResource;

    
    return {handleLinear, handleNearest};
  }
}
