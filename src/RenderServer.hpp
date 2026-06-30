#pragma once
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <string>
struct Vertex {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 uv;
    float texIndex;
};
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

class RenderServer {
public:
      // VAO
    static GLuint gVertexArrayObject;
    // VBO
    static GLuint gVertexBufferObject;
    // pipeline
    static std::unordered_map<std::string, Pipeline> gPipelinePrograms;

    // framebuff for world stuff
    static GLuint worldFBO;
    static GLuint worldColorTex;
    static GLuint worldDepthStencilRBO;
    // framebuff for full screen postProcessing
    static GLuint postFBO;
    static GLuint postColorTex;
    
    static int RSwidth;
    static int RSheight;

    static RenderServer& Get() {
        static RenderServer instance;
        return instance;
    }

    static SDL_Window* window;
    static SDL_GLContext glContext;
    static bool init(const char* title, int width, int height);
    

    static int changeResolution(int width, int height );

    static void render(double dt);
    static void shutdown();

private:
        static GLenum GetShaderTypeFromExtension(const std::string& filePath);

    // slop
    RenderServer() = default;
    static void VertexSpecification();
    static void FrameBufferInit();
    static void InitPipelines();

    //helper for initPipelines
    static void CreateGraphicsPipeline(const PipelineConfig& config);

    static std::string LoadShaderSource(const std::string& filePath);
    static int BindPipeline(const std::string& name);
    static GLuint CompileShader(GLuint type, const std::string& source);

};
