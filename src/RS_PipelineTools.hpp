#pragma once

#include "glad/glad.h"
#include <string>
#include <vector>
struct Pipeline {
  GLuint programID = 0;
};

struct PipelineConfig {
  std::string name;
  std::string vertexPath;
  std::string fragmentPath;
};

class RS_PipelineTool{
  public:

    RS_PipelineTool(const RS_PipelineTool&) = delete;
    RS_PipelineTool& operator=(const RS_PipelineTool&) = delete;
    static RS_PipelineTool& getInstance() {
      static RS_PipelineTool instance;
      return instance;
    }
    static std::string LoadShaderSource(const std::string& filePath);

    GLenum GetShaderTypeFromExtension(const std::string& filePath) {
        if (filePath.find(".vert") != std::string::npos) return GL_VERTEX_SHADER;
        if (filePath.find(".frag") != std::string::npos) return GL_FRAGMENT_SHADER;
        // other here
        return 0; 
    }

  private:
    RS_PipelineTool() = default;
  //       void InitPipelines() {
  //     std::vector<PipelineConfig> configs = {
  //         { "Basic", "assets/shaders/VertexFirst.vert", "assets/shaders/FragmentFirst.frag" },
  //         { "PostProcess", "assets/shaders/FullscreenQuad.vert", "assets/shaders/PostProcess.frag" },
  //         { "ShadowMap", "assets/shaders/Shadow.vert", "assets/shaders/Shadow.frag" }
  //     };
  //
  //     for (const auto& config : configs) {
  //         CreateGraphicsPipeline(config);
  //     }
  // }
};
