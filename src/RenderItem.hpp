#pragma once
#include <glad/glad.h>

#include <glm/mat4x4.hpp>

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

struct Vertex {
  glm::vec3 position;
  glm::vec4 color;
  glm::vec2 uv;
  float texIndex;
};


enum class RenderItemLayer{
  OPAQUE,
  TRANSPARENT,
  UI,
  SKYBOX,
  POSTPROCESS_WORLD,
  POSTPROCESS_SCREEN,
  CAMERA
};
struct RenderItem {
    std::vector<Vertex> localVertices; // quad for 2d , 3d mesh
    std::vector<GLuint> localIndices;  // {0, 1, 2, 2, 3, 0} for a quad
    std::vector<Vertex> worldVertices;
    GLuint textureID; //from textureMgr 
    glm::mat4 modelMatrix; // rot loc scale 
    int shaderPipelineID; //which pipeline? 
    RenderItemLayer layer;
    bool isDirty;
};


