#pragma once
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <string>

#include "RenderItem.hpp"
struct Pipeline {
  GLuint programID = 0;
};

struct PipelineConfig {
  std::string name;
  std::string vertexPath;
  std::string fragmentPath;
};


const size_t MAX_SPRITE_COUNT = 10000;
const size_t MAX_VERTEX_COUNT = MAX_SPRITE_COUNT * 4;

// SDL_Window* RenderServer::window = nullptr;
// SDL_GLContext RenderServer::glContext = nullptr;
//
// GLuint RenderServer::gVertexArrayObject = 0;
// GLuint RenderServer::gVertexBufferObject = 0;
// std::unordered_map<std::string, Pipeline> RenderServer::gPipelinePrograms{};
// GLuint RenderServer::worldFBO=0;
// GLuint RenderServer::worldColorTex=0;
// GLuint RenderServer::worldDepthStencilRBO=0;
// GLuint RenderServer::postFBO=0;
// GLuint RenderServer::postColorTex=0;
//
//
// int RenderServer::RSwidth=0;
// int RenderServer::RSheight=0;


// tmp
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Projection: Map world coordinates directly to screen pixels (0,0 is bottom-left or top-left)


class RenderServer {
  public:
    // VAO
    GLuint gVertexArrayObject=0;
    // VBO
    GLuint gVertexBufferObject=0;
    // IBO
    GLuint gIndexBufferObject = 0;  
    // pipeline
    std::unordered_map<std::string, Pipeline> gPipelinePrograms{};

    // framebuff for world stuff
    GLuint worldFBO=0;
    GLuint worldColorTex=0;
    GLuint worldDepthStencilRBO=0;
    // framebuff for full screen postProcessing
    GLuint postFBO=0;
    GLuint postColorTex=0;


    std::vector<Vertex> bufferedItems;
    int RSwidth=0;
    int RSheight=0;

    static RenderServer& Get() {
      static RenderServer instance;
      return instance;
    }

    SDL_Window* window=nullptr;
    SDL_GLContext glContext=nullptr;
    bool init(const char* title, int width, int height);


    int changeResolution(int width, int height );

    void render(double dt);
    void shutdown();

    std::vector<Vertex> batchBuffer; 
        std::vector<GLuint> bufferedIndices; 


    RenderItemID RegisterItem(const std::vector<Vertex>& localVerts, 
                              const std::vector<GLuint>& localIndices, 
                              GLuint textureID, 
                              const std::string& pipelineName, 
                              RenderItemLayer layer);
    void UpdateTransform(RenderItemID id, const glm::mat4& newMatrix);
    void UnregisterItem(RenderItemID id);

  private:

    std::vector<RenderItem> activeItems;
    RenderItemID NextID = 0;
    GLenum GetShaderTypeFromExtension(const std::string& filePath);

    // slop
    void VertexSpecification();
    void FrameBufferInit();
    void InitPipelines();

    //helper for initPipelines
    void CreateGraphicsPipeline(const PipelineConfig& config);

    std::string LoadShaderSource(const std::string& filePath);
    int BindPipeline(const std::string& name);
    GLuint CompileShader(GLuint type, const std::string& source);

    RenderServer() = default;
    ~RenderServer() = default;
    RenderServer(const RenderServer&) = delete;
    RenderServer& operator=(const RenderServer&) = delete;
    
   
    void bakeVertices(RenderItem& item);

    void Flush();
};
