#pragma once
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <iostream>
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

  private:

    // |||||  SDL2 Items  |||||
    // ||||||||||||||||||||||||
    int RSwidth=0;
    int RSheight=0;

    SDL_Window* window=nullptr;
    SDL_GLContext glContext=nullptr;




    // ||||| OpenGL Items |||||
    // |||||||||||||||||||||||| 
    // VAO
    GLuint gVertexArrayObject=0;
    // VBO
    GLuint gVertexBufferObject=0;
    // IBO
    GLuint gIndexBufferObject = 0;  
    // Instance VBO 
    GLuint gInstanceVBO = 0;
    // pipeline
    std::unordered_map<std::string, Pipeline> gPipelinePrograms{};

    // framebuff for world stuff
    GLuint worldFBO=0;
    GLuint worldColorTex=0;
    GLuint worldDepthStencilRBO=0;
    // framebuff for full screen postProcessing
    GLuint postFBO=0;
    GLuint postColorTex=0;

    // ||||| instange management |||||
    // ||||||||||||||||||||||||||||||| 

    // buckets rework
    std::vector<RenderInstance> opaqueQueue;
    std::vector<RenderInstance> transparentQueue;
    std::vector<RenderInstance> uiQueue;
    std::unordered_map<RenderItemID, RenderInstance*> instanceDirectory;
    RenderItemID NextID = 0;    


    // ||||| Function!!! ||||
    // |||||||||||||||||||||
    //  TODO: sort this LOL

    GLenum GetShaderTypeFromExtension(const std::string& filePath);

    // init 
    void VertexSpecification();
    void FrameBufferInit();
    void InitPipelines();
    // init helpers
    //helper for initPipelines
    void CreateGraphicsPipeline(const PipelineConfig& config);
    std::string LoadShaderSource(const std::string& filePath);
    int BindPipeline(const std::string& name);
    GLuint CompileShader(GLuint type, const std::string& source);


    
    // render stuff! mostly helpers
    void DrawFullScreenQuad();
    void RenderQueue(const std::vector<RenderInstance>& queue);
    void FlushInstancedBatch(GLuint vaoID, GLuint textureID, GLuint indexCount, const std::vector<glm::mat4>& matrices);
    void FlushInstancedBatch2d(GLuint textureID, const std::vector<glm::mat4>& matrices);
  

    // class oop stuff
    RenderServer() = default;
    ~RenderServer() { shutdown(); }
    RenderServer(const RenderServer&) = delete;
    RenderServer& operator=(const RenderServer&) = delete;


  public:

    
    // |||||  MiscConfig  |||||
    // ||||||||||||||||||||||||
    bool only2D = true;
    bool doWPP = true;
    bool doSPP = true;


    static RenderServer& Get() {
      static RenderServer instance;
      return instance;
    }



    bool init(const char* title, int width, int height);
    int changeResolution(int width, int height );
    void render(double dt);
    void shutdown();



    // regestering
    void SubmitInstance(const RenderInstance& instance);
    void ClearQueues();


    // std::vector<Vertex> batchBuffer; 
    // std::vector<GLuint> bufferedIndices; 


    // RenderItemID RegisterItem(const std::vector<Vertex>& localVerts, 
    //                           const std::vector<GLuint>& localIndices, 
    //                           GLuint textureID, 
    //                           const std::string& pipelineName, 
    //                           RenderItemLayer layer);
    // void UpdateTransform(RenderItemID id, const glm::mat4& newMatrix);
    // void UnregisterItem(RenderItemID id);

};
