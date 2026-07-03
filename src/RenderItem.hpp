#pragma once
#include <glad/glad.h>

#include <glm/mat4x4.hpp>

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <string>
#include "ansi_control.hpp"
struct Vertex {
  glm::vec3 position;
  glm::vec4 color;
  glm::vec2 uv;
  GLuint64 linHandle;
  GLuint64 nearHandle;
  // float texIndex;
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

enum SamplerType {
  LINEAR=true,
  NEAREST=false,
};
using RenderItemID = uint32_t;

#include <iostream>
#include <string>
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp> // Required to print glm::mat4
struct RenderInstance { 
    RenderItemID id; //who
    glm::mat4 globalTransform; // where
    // GLuint textureID = 0; // which texture 
    GLuint64 linHandle =0;
    GLuint64 nearHandle=0;
    GLuint vaoID=0; // the shape (assumes 0 for universal for now)
    GLuint indexCount = 6;
    std::string pipelineName = "MVP";
    RenderItemLayer layer = RenderItemLayer::OPAQUE;
    SamplerType LinearNearest = SamplerType::LINEAR;
    bool isDirty = true;
        friend std::ostream& operator<<(std::ostream& os, const RenderInstance& ri) {
        os << "RenderInstance {\n"
           << "  id: " << ri.id << ",\n"
           << "  globalTransform: " << glm::to_string(ri.globalTransform) << ",\n"
           << "  LinearHandle: " << ri.linHandle << ",\n"
           << "  NearestHandle: " << ri.linHandle << ",\n"
           << "  vaoID: " << ri.vaoID << ",\n"
           << "  indexCount: " << ri.indexCount << ",\n"
           << "  pipelineName: \"" << ri.pipelineName << "\",\n"
           << "  layer: " << static_cast<int>(ri.layer) << ",\n"
           << "  isDirty: " << (ri.isDirty ? "true" : "false") << "\n"
           << "}";
        term::move_up(12,true);
        return os;
    }
};

                                   

    // std::vector<Vertex> localVertices; // quad for 2d , 3d mesh
    // std::vector<GLuint> localIndices;  // {0, 1, 2, 2, 3, 0} for a quad
    // std::vector<Vertex> worldVertices;
    // glm::mat4 modelMatrix = glm::mat4(1.0f); // rot loc scale 
