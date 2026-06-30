#include "RenderServer.hpp"
#include "RenderItem.hpp"
#include "glad/glad.h"
#include <SDL_stdinc.h>
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


void RenderServer::render(double dt) {

  // sort bucket by texture 
  std::sort(opaqueQueue.begin(), opaqueQueue.end(), [](const RenderInstance& a, const RenderInstance& b) {
        return a.textureID < b.textureID;
    });

  // 2. sort transparent by camera dist? see notes
  // TODO: this

  // |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
  // |-|-|-|             Opaque Layer Pass               |-|-|-|
  // |-|-|-|   Needs: WFB, Depth Test, Depth Mask Write  |-|-|-|
  // |-|-|-|  Toggle: WFB, Depth Test, Depth Mask Write  |-|-|-|
  // |-|-|-|            Doesnt Use: Blend                |-|-|-|
  // |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|

  glBindFramebuffer(GL_FRAMEBUFFER, worldFBO);
  glEnable(GL_DEPTH_TEST); // test
  glDepthMask(GL_TRUE); // mask
  glDisable(GL_BLEND); // blend
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f); 


  int pickedPipeline = BindPipeline("MVP"); 
  if (pickedPipeline == -1) return;
  RenderQueue(opaqueQueue);


  // |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
  // |-|-|-|          Transparent Layer Pass             |-|-|-|
  // |-|-|-|     Needs: WFB, Depth Test, Blend           |-|-|-|
  // |-|-|-|    Toggle: Mask Write Off, Blend On         |-|-|-|
  // |-|-|-|            Doesnt Use:                      |-|-|-|
  // |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
  // uses: worldFramebuffer, Blend, Depth Test 
  // doesnt: No Depth Mask Write 
  
  
  // frame buffer + depth test still bound.
  glDepthMask(GL_FALSE); // mask
  glEnable(GL_BLEND); // blend
  pickedPipeline = BindPipeline("MVP"); 
  if (pickedPipeline == -1) return;
  RenderQueue(transparentQueue); 


  
  // |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
  // |-|-|-|          Post-Process World Pass             |-|-|-|
  // |-|-|-|    Needs: postFBO, World Color Texture       |-|-|-|
  // |-|-|-|  Toggle: FBO BOundDepth Test Off, Blend Off  |-|-|-|
  // |-|-|-|       Doesnt Use: Depth Mask Write           |-|-|-|
  // |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
  glBindFramebuffer(GL_FRAMEBUFFER, postFBO);
  glClear(GL_COLOR_BUFFER_BIT); // only needs color 
  glDisable(GL_DEPTH_TEST); 
  // glDepthMask(GL_FALSE); // shouldnt need reset but 
  glDisable(GL_BLEND);
  
  pickedPipeline = BindPipeline("WorldPostProcess");
  if (pickedPipeline == -1) return;

  float minX = 0.4f;
  float minY = 0.4f;
  float maxX = 0.6f;
  float maxY = 0.6f;
  GLint boundsLoc = glGetUniformLocation(pickedPipeline, "u_InversionBounds");
  if (boundsLoc != -1) {
      glUniform4f(boundsLoc, minX, minY, maxX, maxY);
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,worldColorTex); // bind texture from worldFBO

  DrawFullScreenQuad();

  // |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
  // |-|-|-|               UI Overlay Pass               |-|-|-|
  // |-|-|-|        Needs: postFBO, Blend                |-|-|-|
  // |-|-|-|       Toggle: Depth Test Off, Blend On      |-|-|-|
  // |-|-|-|              Doesnt Use: Depth Mask Write  |-|-|-|
  // |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
  // keep the postFBO bound cause we just write over it  
  
  // glDisable(GL_DEPTH_TEST); // UI ignores depth  no need to reset
  glEnable(GL_BLEND); // it does need transparency. probably?

  pickedPipeline = BindPipeline("MVP");
  if (pickedPipeline == -1) return;
  RenderQueue(uiQueue);

  // |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
  // |-|-|-|          Post-Process Screen Pass           |-|-|-|
  // |-|-|-|   Needs: Screen (0), PostFBO Color Texture  |-|-|-|
  // |-|-|-|  Toggle: Screen FBO, Depth Off, Blend Off   |-|-|-|
  // |-|-|-|            Doesnt Use:                      |-|-|-|
  // |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
  // remember the ping->pong system
  // load screen out as buffer we write to, pass it what we've drawn so far as a value 

  glBindFramebuffer(GL_FRAMEBUFFER, 0); // bind directly to screen
  glClear(GL_COLOR_BUFFER_BIT); // doesnt need to clear depth
  // glDisable(GL_DEPTH_TEST);  // should have stayed off so
  glDisable(GL_BLEND);

  pickedPipeline = BindPipeline("ScreenPostProcess");
  if (pickedPipeline == -1) return;
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, postColorTex); // Texture from postProcessFBO
  DrawFullScreenQuad();

  
  SDL_GL_SwapWindow(window);
  ClearQueues();

} 
  

  // glm::mat4 projection = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f, -1.0f, 1.0f);
  // GLint locProj = glGetUniformLocation(pickedPipeline, "u_Projection");
  // glUniformMatrix4fv(locProj, 1, GL_FALSE, glm::value_ptr(projection));
  //
  //
  // GLint locTex = glGetUniformLocation(pickedPipeline, "u_Texture");
  // glUniform1i(locTex, 0);
  //
  //
  //
  // GLuint currentTexture = 0;
  // GLuint vertexOffset = 0;
  //
  // bufferedItems.clear();
  // bufferedIndices.clear();
  //
  // glBindVertexArray(gVertexArrayObject);
  //
  // for (auto& item : activeItems) {
  //   // std::cout<<"renderTick on id = " <<item.id<<std::endl;
  //   if (item.isDirty) {
  //     bakeVertices(item); 
  //     item.isDirty = false;
  //   }
  //
  //   if (item.textureID != currentTexture) {
  //     Flush(); 
  //     currentTexture = item.textureID;
  //     glActiveTexture(GL_TEXTURE0); 
  //     glBindTexture(GL_TEXTURE_2D, currentTexture);
  //     vertexOffset = 0;
  //     // std::cout<<"  swapped to textureid = "<<currentTexture<<std::endl;
  //   }
  //
  //   for (const auto& v : item.worldVertices) {
  //     // std::cout<<"  pushedVert = " <<item.worldVertices[0].position.x <<item.worldVertices[0].position.y <<item.worldVertices[0].position.z <<std::endl;
  //
  //     bufferedItems.push_back(v);
  //   }
  //
  //   for (GLuint index : item.localIndices) {
  //           // std::cout<<"  pushedindex = " <<item.localIndices[0] <<std::endl;
  //
  //     bufferedIndices.push_back(index + vertexOffset);
  //   }
  //
  //   vertexOffset += item.worldVertices.size();
  // }
  //
  // Flush();


// void RenderServer::render(double dt){
//
//   // glDisable(GL_DEPTH_TEST);
//   // glDisable(GL_CULL_FACE);
//   glEnable(GL_DEPTH_TEST);
//   glDepthFunc(GL_LESS);
//   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//   //
//   // glUseProgram(gPipelinePrograms["Basic"]);
//
//   int pickedPipeline = BindPipeline("MVP");
//   if(pickedPipeline == -1){
//     std::cerr<<"PipelinePickingFailed!! \n";
//   }
//   glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//
//
//   std::vector<Vertex> testVertices = {
//     { {-0.6f, -0.5f, 0.0f}, {1.0f, 0.4f, 0.0f, 1.0f}, {0.0f, 0.0f}, 0.0f },
//     { { 0.0f,  0.6f, 0.0f}, {1.0f, 0.4f, 0.0f, 1.0f}, {0.5f, 1.0f}, 0.0f }, 
//     { { 0.6f, -0.5f, 0.0f}, {1.0f, 0.4f, 0.0f, 1.0f}, {1.0f, 0.0f}, 0.0f },
//
//     { {-0.2f, -0.3f, 0.0f}, {0.0f, 0.8f, 1.0f, 1.0f}, {0.0f, 0.0f}, 0.0f }, 
//     { { 0.4f,  0.8f, 0.0f}, {0.0f, 0.8f, 1.0f, 1.0f}, {0.5f, 1.0f}, 0.0f }, 
//     { { 1.0f, -0.3f, 0.0f}, {0.0f, 0.8f, 1.0f, 1.0f}, {1.0f, 0.0f}, 0.0f }
//   };
//
//   GLint locToPass = glGetUniformLocation(static_cast<GLuint>(pickedPipeline), "u_Time");
//   glUniform1f(locToPass, static_cast<float>(dt));
//   glBindVertexArray(gVertexArrayObject);
//
//   glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
//   glBufferSubData(
//       GL_ARRAY_BUFFER, 
//       0, 
//       testVertices.size() * sizeof(Vertex), 
//       testVertices.data()
//       );
//   // glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
//
//   // glDrawArrays(GL_TRIANGLES, 0, 3);  
//
//   glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(testVertices.size()));
//
//
//   SDL_GL_SwapWindow(window);
//
//   glBindVertexArray(0);
//   glBindBuffer(GL_ARRAY_BUFFER, 0);
// }



void RenderServer::Flush() {

  // std::cout<<"called a flush with count of:" << bufferedItems.size() << " and " << bufferedIndices.size()<<std::endl;
  if (bufferedItems.empty() || bufferedIndices.empty()) return;
  glBindVertexArray(gVertexArrayObject);
  glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
  glBufferSubData(GL_ARRAY_BUFFER, 0, bufferedItems.size() * sizeof(Vertex), bufferedItems.data());
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferObject);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, bufferedIndices.size() * sizeof(GLuint), bufferedIndices.data());

  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(bufferedIndices.size()), GL_UNSIGNED_INT, nullptr);

  bufferedItems.clear();
  bufferedIndices.clear();
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

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
  // first, opengl stuff
  // unbind all?
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glUseProgram(0);

  //delete each pipeline
  for (auto& [name, pipeline] : gPipelinePrograms) {
    if (pipeline.programID != 0) {
        glDeleteProgram(pipeline.programID);
        pipeline.programID = 0;
    }
  }
  gPipelinePrograms.clear();
  // VAO, VBO, IBO, Framebuffers. ew
  if (gVertexArrayObject != 0) {
    glDeleteVertexArrays(1, &gVertexArrayObject);
    gVertexArrayObject = 0;
  }
  if (gVertexBufferObject != 0) {
      glDeleteBuffers(1, &gVertexBufferObject);
      gVertexBufferObject = 0;
  }
  if (gIndexBufferObject != 0) {
      glDeleteBuffers(1, &gIndexBufferObject);
      gIndexBufferObject = 0;
  }

  if (worldFBO != 0) {
      glDeleteFramebuffers(1, &worldFBO);
      worldFBO = 0;
  }
  if (worldColorTex != 0) {
      glDeleteTextures(1, &worldColorTex);
      worldColorTex = 0;
  }
  if (worldDepthStencilRBO != 0) {
      glDeleteRenderbuffers(1, &worldDepthStencilRBO);
      worldDepthStencilRBO = 0;
  }

  if (postFBO != 0) {
      glDeleteFramebuffers(1, &postFBO);
      postFBO = 0;
  }
  if (postColorTex != 0) {
      glDeleteTextures(1, &postColorTex);
      postColorTex = 0;
  }


  // then sdl2 stuff
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


  // IBO 
  glGenBuffers(1, &gIndexBufferObject);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferObject);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_SPRITE_COUNT * 6 * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);


  //unbindbind?
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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
    { "WorldPostProcess", "../assets/shaders/WorldPostProcess.frag",  "../assets/shaders/PostProcess.vert"},  
    { "ScreenPostProcess", "../assets/shaders/ScreenPostProcess.frag",  "../assets/shaders/PostProcess.vert"},  
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



void DrawFullScreenQuad() {
    static GLuint dummyVAO = 0;
    
    if (dummyVAO == 0) {
        glGenVertexArrays(1, &dummyVAO);
    }

    glBindVertexArray(dummyVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}



// void RenderServer::bakeVertices(RenderItem& item){
//   item.worldVertices.clear(); 
//
//   for (const auto& localVert : item.localVertices) {
//     Vertex worldVert = localVert;
//
//     glm::vec4 transformedPos = item.modelMatrix * glm::vec4(localVert.position, 1.0f);
//     worldVert.position = glm::vec3(transformedPos);
//
//     item.worldVertices.push_back(worldVert);
//   }
// }

// RenderServer.cpp


void RenderServer::SubmitInstance(const RenderInstance& instance) {
      switch(instance.layer) {
        case RenderItemLayer::OPAQUE: opaqueQueue.push_back(instance); break;
        case RenderItemLayer::TRANSPARENT: transparentQueue.push_back(instance); break;
        case RenderItemLayer::UI: uiQueue.push_back(instance); break;
        default: std::cerr<<"error? didnt handle yet lol (submitinstance)"<<std::endl; break;
      }
    }

void RenderServer::ClearQueues(){
        opaqueQueue.clear();
      transparentQueue.clear();
      uiQueue.clear();
   

}

// RenderItemID RenderServer::RegisterItem(const std::vector<Vertex>& localVerts, 
//     const std::vector<GLuint>& localIndices, 
//     GLuint textureID, 
//     const std::string& pipelineName, 
//     RenderItemLayer layer) 
// {
//   RenderItem newItem;
//   newItem.id = NextID++;
//   newItem.localVertices = localVerts;
//   newItem.localIndices = localIndices;
//
//   newItem.worldVertices.resize(localVerts.size()); 
//
//   newItem.textureID = textureID;
//   newItem.pipelineName = pipelineName;
//   newItem.layer = layer;
//   newItem.modelMatrix = glm::mat4(1.0f);
//   newItem.isDirty = true;
//
//   activeItems.push_back(newItem);
//   std::cout<<"registered at id = "<<newItem.id<<std::endl;
//   return newItem.id; 
// }

// void RenderServer::UpdateTransform(RenderItemID id, const glm::mat4& newMatrix) {
//   for (auto& item : activeItems) {
//     if (item.id == id) {
//       item.modelMatrix = newMatrix;
//       item.isDirty = true;
//       break;
//     }
//   }
// }  
