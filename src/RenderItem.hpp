#pragma once
#include <glad/glad.h>

#include <glm/mat4x4.hpp>

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <string>

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
using RenderItemID = uint32_t;

struct RenderItem { 
    RenderItemID id;
    std::vector<Vertex> localVertices; // quad for 2d , 3d mesh
    std::vector<GLuint> localIndices;  // {0, 1, 2, 2, 3, 0} for a quad
    std::vector<Vertex> worldVertices;
    GLuint textureID = 0; //from textureMgr 
    glm::mat4 modelMatrix = glm::mat4(1.0f); // rot loc scale 
    std::string pipelineName = "MVP";
    RenderItemLayer layer = RenderItemLayer::OPAQUE;
    bool isDirty = true;
};


