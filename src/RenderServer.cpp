#include "RenderServer.hpp"
#include "glad/glad.h"
#include <iostream>

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include "TextureManager.hpp"


int RenderServer::BindPipeline(const std::string& name) {
  auto it = gPipelinePrograms.find(name);
  if (it != gPipelinePrograms.end()) {
    glUseProgram(it->second.programID);
    return it->second.programID;
  } else {
    std::cerr << "Pipeline " << name << " not found!\n";
    return -1;
  }
}

void RenderServer::render(double dt){
  // glDisable(GL_DEPTH_TEST);
  // glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  //
  // glUseProgram(gPipelinePrograms["Basic"]);

  int pickedPipeline = BindPipeline("MVP");
  if(pickedPipeline == -1){
    std::cerr<<"PipelinePickingFailed!! \n";
  }
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


  std::vector<Vertex> testVertices = {
    { {-0.6f, -0.5f, 0.0f}, {1.0f, 0.4f, 0.0f, 1.0f}, {0.0f, 0.0f}, 0.0f },
    { { 0.0f,  0.6f, 0.0f}, {1.0f, 0.4f, 0.0f, 1.0f}, {0.5f, 1.0f}, 0.0f }, 
    { { 0.6f, -0.5f, 0.0f}, {1.0f, 0.4f, 0.0f, 1.0f}, {1.0f, 0.0f}, 0.0f },

    { {-0.2f, -0.3f, 0.0f}, {0.0f, 0.8f, 1.0f, 1.0f}, {0.0f, 0.0f}, 0.0f }, 
    { { 0.4f,  0.8f, 0.0f}, {0.0f, 0.8f, 1.0f, 1.0f}, {0.5f, 1.0f}, 0.0f }, 
    { { 1.0f, -0.3f, 0.0f}, {0.0f, 0.8f, 1.0f, 1.0f}, {1.0f, 0.0f}, 0.0f }
  };

  GLint locToPass = glGetUniformLocation(static_cast<GLuint>(pickedPipeline), "u_Time");
  glUniform1f(locToPass, static_cast<float>(dt));
  glBindVertexArray(gVertexArrayObject);

  glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
  glBufferSubData(
      GL_ARRAY_BUFFER, 
      0, 
      testVertices.size() * sizeof(Vertex), 
      testVertices.data()
      );
  // glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);

  // glDrawArrays(GL_TRIANGLES, 0, 3);  

  glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(testVertices.size()));


  SDL_GL_SwapWindow(window);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}




bool RenderServer::init(const char* title, int width, int height){
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr<<"SDL Init Error: %s\n"<<SDL_GetError()<<"\n";
    return false;
  }





  std::cout<<gVertexArrayObject<< " " << gVertexBufferObject << " " << std::endl;
  // // Set core OpenGL context parameters before window generation
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  // stability
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
  // Color! Framebuff fun
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
  // anti alias
  //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

  // debug
  // glGetError();
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
  //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
  window = SDL_CreateWindow(title, 
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
      width, height, 
      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALWAYS_ON_TOP);
  if (!window) {
    std::cerr<<"Window Error: %s\n"<<SDL_GetError()<<"\n";
    return false;
  }

  glContext = SDL_GL_CreateContext(window);
  if (!glContext) {
    std::cerr<<"OpenGL Context Error: %s\n"<<SDL_GetError()<<"\n";
    return false;
  }
  if(!SDL_GL_MakeCurrent(window, glContext)){

  }

  //init GLAD
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    std::cerr<<"Failed to initialize Glad: most recent SDL Err: %s\n"<<SDL_GetError()<<"\n";
    return -1;
  }
  // 0 = vsync off, 1 = vsync, -1 = adaptive vsync (?)
  SDL_GL_SetSwapInterval(1); 

  glViewport(0,0,width,height);


  // sys info 
  std::cout<< "-Vendor "            << glGetString(GL_VENDOR)    << 
    "\n-Renderer "        << glGetString(GL_RENDERER) <<
    "\n-Version "         << glGetString(GL_VERSION)  <<
    "\n-Shading Language "<< glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  // glBindBuffer()
  // glBindBufer


  TextureManager::Get().init();
  RSwidth = width;
  RSheight = height;
  VertexSpecification();
  FrameBufferInit();
  InitPipelines();
  return true;
}



int RenderServer::changeResolution(int width, int height) {

  // ig bro
  while (glGetError() != GL_NO_ERROR) {
    std::cerr << "[Warning !!! SUPER SERIOUS!!] Cleared unrelated OpenGL error prior to resolution change.\n";
  }
  SDL_ClearError();

  SDL_SetWindowSize(window, width, height);
  SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  glViewport(0,0,width, height);


  const char* sdlError = SDL_GetError();
  if (sdlError && sdlError[0] != '\0') {
    std::cerr << "[Critical] SDL Window modification failed: " << sdlError << "\n";
    SDL_ClearError(); 
    return -1;      
  }

  GLenum glErr = glGetError();
  if (glErr != GL_NO_ERROR) {
    std::cerr << "[Critical] OpenGL viewport assignment failed. Error Code: " << glErr << "\n";
    return -2;    
  }


  RSwidth = width;
  RSheight = height;
  // i should redeclare if size changes?
  FrameBufferInit();
  return 0;
}

void RenderServer::shutdown(){
  if (glContext) SDL_GL_DeleteContext(glContext);
  if (window) SDL_DestroyWindow(window);
  SDL_Quit();

}

void RenderServer::VertexSpecification(){

  // steps: create + bind VAO, VBO 
  glGenVertexArrays(1,&gVertexArrayObject);
  glBindVertexArray(gVertexArrayObject);
  glGenBuffers(1, &gVertexBufferObject);
  glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);

  // notible: marked as dynamic (will be written to often?, preealloc'd big size;
  glBufferData(GL_ARRAY_BUFFER, MAX_VERTEX_COUNT * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

  // vertex struct is now
  // {x,y,z}, {r,g,b,a}, {u,v},{texture slot id}
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texIndex));

  //unbindbind?
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void RenderServer::FrameBufferInit(){
  //cleanup?
  if (worldFBO) glDeleteFramebuffers(1, &worldFBO);
  if (worldColorTex) glDeleteTextures(1, &worldColorTex);
  if (worldDepthStencilRBO) glDeleteRenderbuffers(1, &worldDepthStencilRBO);
  if (postFBO) glDeleteFramebuffers(1, &postFBO);
  if (postColorTex) glDeleteTextures(1, &postColorTex);


  // world FB
  // gen+bind
  glGenFramebuffers(1, &worldFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, worldFBO);

  //colortexture
  // gen, bind
  glGenTextures(1, &worldColorTex);
  glBindTexture(GL_TEXTURE_2D, worldColorTex);
  // create
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, RSwidth, RSheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  //configs
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, worldColorTex, 0);


  //depth buffer
  //gen+bind
  glGenRenderbuffers(1, &worldDepthStencilRBO);
  glBindRenderbuffer(GL_RENDERBUFFER, worldDepthStencilRBO);
  //create
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, RSwidth, RSheight);
  //config?
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, worldDepthStencilRBO);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr<<"error initializing frameBuffer (world), see FrameBufferInit()!\n";
  }


  // the postprocess Framebuffer (screen)
  glGenFramebuffers(1, &postFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, postFBO);

  //colortexture
  glGenTextures(1, &postColorTex);
  glBindTexture(GL_TEXTURE_2D, postColorTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, RSwidth, RSheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postColorTex, 0);

  //no need for depth b/c its a 2d quad drawn ondo
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr<<"error initializing frameBuffer (screen), see FrameBufferInit()!\n";
  }

  //unbind
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderServer::InitPipelines(){
  // std::vector<std::string> LoadedShaders={
  //   "assets/shaders/FragmentFirst.frag",
  //   "assets/shaders/VertexFirst.vert"
  // };
  // auto it = LoadedShaders.begin();
  // for (auto it = LoadedShaders.begin(); it != LoadedShaders.end(); ++it) {
  //
  // }
  std::vector<PipelineConfig> configs = {
    { "Basic", "../assets/shaders/VertexFirst.vert", "../assets/shaders/FragmentFirst.frag" },
    { "MVP", "../assets/shaders/VertMVP.vert", "../assets/shaders/FragMVP.frag" },
    // { "PostProcess", "assets/shaders/FullscreenQuad.vert", "assets/shaders/PostProcess.frag" },
    // { "ShadowMap", "assets/shaders/Shadow.vert", "assets/shaders/Shadow.frag" }
  };

  for (const auto& config : configs) {
    CreateGraphicsPipeline(config);
  }
}

void RenderServer::CreateGraphicsPipeline(const PipelineConfig& config){
  //create
  GLuint programObject = glCreateProgram();
  std::vector<GLuint> compiledShaders; // out
  std::vector<std::string> paths = { config.vertexPath, config.fragmentPath };
  //safely loop thru 
  for (const std::string& path : paths) {
    if (path.empty()) continue;

    GLenum type = GetShaderTypeFromExtension(path);
    if (type == 0) {
      std::cerr << "Unknown shader extension for file: " << path << "\n";
      continue;
    }

    // load, compile
    std::string source = LoadShaderSource(path);
    GLuint shader = CompileShader(type, source); 

    glAttachShader(programObject, shader);
    compiledShaders.push_back(shader);
  }

  glLinkProgram(programObject);
  glValidateProgram(programObject);
  GLint success;
  glGetProgramiv(programObject, GL_LINK_STATUS, &success);
  if (!success) {
    GLint logLength;
    glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> infoLog(logLength);
    glGetProgramInfoLog(programObject, logLength, nullptr, infoLog.data());

    std::cerr << "Shader Program Linking Error:\n" << infoLog.data() << std::endl;
  }

  // cleanup
  for (GLuint shader : compiledShaders) {
    glDetachShader(programObject, shader);
    glDeleteShader(shader);
  }

  // Save the finished pipeline into our server's map
  Pipeline pipeline;
  pipeline.programID = programObject;
  RenderServer::gPipelinePrograms[config.name] = pipeline;

}


std::string RenderServer::LoadShaderSource(const std::string &filePath){
  std::ifstream fileStream(filePath);
  if (!fileStream.is_open()) {
    std::cerr << "Error: Could not open shader file at " << filePath << std::endl;
    return "";
  }

  std::stringstream stringStream;
  stringStream << fileStream.rdbuf(); 
  return stringStream.str(); 
}

GLenum RenderServer::GetShaderTypeFromExtension(const std::string& filePath) {
  if (filePath.find(".vert") != std::string::npos) return GL_VERTEX_SHADER;
  if (filePath.find(".frag") != std::string::npos) return GL_FRAGMENT_SHADER;
  // other here
  return 0; 
}

GLuint RenderServer::CompileShader(GLuint type, const std::string& source){
  //oh boy 
  GLuint shaderObject = 0;
  if(type==GL_VERTEX_SHADER){
    shaderObject = glCreateShader(GL_VERTEX_SHADER);

  }else if (type==GL_FRAGMENT_SHADER){
    shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
  }
  const char* str = source.c_str(); // its a C library so.
  glShaderSource(shaderObject,1,&str,nullptr);
  glCompileShader(shaderObject);

  GLint success;
  glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLint logLength;
    glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> infoLog(logLength);
    glGetShaderInfoLog(shaderObject, logLength, nullptr, infoLog.data());

    std::cerr << "Shader Compilation Error (" 
      << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") 
      << "):\n" << infoLog.data() << std::endl;
  }
  return shaderObject;
}

    void RenderServer::bakeVertices(RenderItem& item){
    item.worldVertices.clear(); // A cached vector of the transformed verts
    
    for (const auto& localVert : item.localVertices) {
        Vertex worldVert = localVert;
        
        // Transform the local vertex by the node's model matrix ON THE CPU
        glm::vec4 transformedPos = item.modelMatrix * glm::vec4(localVert.position, 1.0f);
        worldVert.position = glm::vec3(transformedPos);
        
        item.worldVertices.push_back(worldVert);

