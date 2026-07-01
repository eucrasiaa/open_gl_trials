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
  
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); 

  // sort bucket by texture 
  // std::sort(opaqueQueue.begin(), opaqueQueue.end(), [](const RenderInstance& a, const RenderInstance& b) {
  //       return a.textureID < b.textureID;
  //   });

std::sort(opaqueQueue.begin(), opaqueQueue.end(), [](const RenderInstance& a, const RenderInstance& b) {
    if (a.vaoID != b.vaoID) {
        return a.vaoID < b.vaoID; // mesh types
    }
    return a.textureID < b.textureID; // cubes by texture
});
  // 2. sort transparent by camera dist? see notes
  // TODO: eventually tie to camera pos i think
  std::sort(transparentQueue.begin(), transparentQueue.end(), [](const RenderInstance& a, const RenderInstance& b) {
    // z translation, Highest Z -> Lowest Z (back to front) 
    return a.globalTransform[3].z < b.globalTransform[3].z; 
  });

  // |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
  // |-|-|-|             Opaque Layer Pass               |-|-|-|
  // |-|-|-|   Needs: WFB, Depth Test, Depth Mask Write  |-|-|-|
  // |-|-|-|  Toggle: WFB, Depth Test, Depth Mask Write  |-|-|-|
  // |-|-|-|            Doesnt Use: Blend                |-|-|-|
  // |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|

  // glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, worldFBO);
  // glEnable(GL_DEPTH_TEST); // test
  glDisable(GL_DEPTH_TEST);
  // glDepthMask(GL_TRUE); // mask
  glDisable(GL_BLEND); // blend
  //trial
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  


  int pickedPipeline = BindPipeline("MVP"); 
  if (pickedPipeline == -1) return;


  // uniforms here!
  setProjectionUniform(pickedPipeline);

  GLint locTex = glGetUniformLocation(pickedPipeline, "u_Texture"); 
  if (locTex != -1) {
      glUniform1i(locTex, 0); // Tell the sampler to read from GL_TEXTURE0
  }

  // camera matrix would go in here
  RenderQueue(opaqueQueue);

  // SDL_GL_SwapWindow(window);
  // ClearQueues();
  // return;
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
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  pickedPipeline = BindPipeline("MVP"); 
  if (pickedPipeline == -1) return;

  // glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(RSwidth), 0.0f, static_cast<float>(RSheight), -1.0f, 1.0f);
  setProjectionUniform(pickedPipeline);
  locTex = glGetUniformLocation(pickedPipeline, "u_Texture"); 
  if (locTex != -1) {
      glUniform1i(locTex, 0); 
  }
  RenderQueue(transparentQueue); 


  
  // |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
  // |-|-|-|          Post-Process World Pass             |-|-|-|
  // |-|-|-|    Needs: postFBO, World Color Texture       |-|-|-|
  // |-|-|-|  Toggle: FBO BOundDepth Test Off, Blend Off  |-|-|-|
  // |-|-|-|       Doesnt Use: Depth Mask Write           |-|-|-|
  // |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
  if(doWPP){
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glBindFramebuffer(GL_FRAMEBUFFER, postFBO);
    glClear(GL_COLOR_BUFFER_BIT); // only needs color 
    glDisable(GL_DEPTH_TEST); 
    // glDepthMask(GL_FALSE); // shouldnt need reset but 
    glDisable(GL_BLEND);
    
    pickedPipeline = BindPipeline("WorldPostProcess");
    if (pickedPipeline == -1) return;


    GLint boundsLoc = glGetUniformLocation(pickedPipeline, "u_InversionShift");
    if (boundsLoc != -1) {
      glUniform1f(boundsLoc, static_cast<float>(dt));  
      // glUniform4f(boundsLoc, minX, minY, maxX, maxY);
    }
 
    // float minX = 0.4f;
    // float minY = 0.4f;
    // float maxX = 0.6f;
    // float maxY = 0.6f;
    // // Bonus: for testing
    // float frequency = 1.0f;
    // float moveRadius = 0.3f; 
    // float shiftX = std::sin(dt * frequency) * moveRadius;
    // float shiftY = std::cos(dt * frequency) * moveRadius;
    //
    // float currentMinX = minX + shiftX;
    // float currentMinY = minY + shiftY;
    // float currentMaxX = maxX + shiftX;
    // float currentMaxY = maxY + shiftY;
    // // end bonus
    // GLint boundsLoc = glGetUniformLocation(pickedPipeline, "u_InversionBounds");
    // if (boundsLoc != -1) {
    //    glUniform4f(boundsLoc, currentMinX, currentMinY, currentMaxX, currentMaxY);  
    //   // glUniform4f(boundsLoc, minX, minY, maxX, maxY);
    // }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,worldColorTex); // bind texture from worldFBO

    DrawFullScreenQuad();
  }
  else{
    // glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // should be set above
    glBindFramebuffer(GL_READ_FRAMEBUFFER, worldFBO); 
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postFBO);
    
    glDisable(GL_SCISSOR_TEST); 
    
    glBlitFramebuffer(
      0, 0, RSwidth, RSheight,
      0, 0, RSwidth, RSheight, 
      GL_COLOR_BUFFER_BIT,
      GL_NEAREST
    );
  }
  // |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
  // |-|-|-|               UI Overlay Pass               |-|-|-|
  // |-|-|-|        Needs: postFBO, Blend                |-|-|-|
  // |-|-|-|       Toggle: Depth Test Off, Blend On      |-|-|-|
  // |-|-|-|              Doesnt Use: Depth Mask Write  |-|-|-|
  // |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
  // keep the postFBO bound cause we just write over it  
  
  glBindFramebuffer(GL_FRAMEBUFFER, postFBO); // incase wpp skipped, ensure
  glDisable(GL_DEPTH_TEST); // UI ignores depth  no need to reset
  glEnable(GL_BLEND); // it does need transparency. probably?
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
  pickedPipeline = BindPipeline("MVP");
  if (pickedPipeline == -1) return;


  setProjectionUniform(pickedPipeline);


  RenderQueue(uiQueue);

  // |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
  // |-|-|-|          Post-Process Screen Pass           |-|-|-|
  // |-|-|-|   Needs: Screen (0), PostFBO Color Texture  |-|-|-|
  // |-|-|-|  Toggle: Screen FBO, Depth Off, Blend Off   |-|-|-|
  // |-|-|-|            Doesnt Use:                      |-|-|-|
  // |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
  // remember the ping->pong system
  // load screen out as buffer we write to, pass it what we've drawn so far as a value 
  if(doSPP){
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // bind directly to screen
    glClear(GL_COLOR_BUFFER_BIT); // doesnt need to clear depth
    // glDisable(GL_DEPTH_TEST);  // should have stayed off so
    glDisable(GL_BLEND);

    pickedPipeline = BindPipeline("ScreenPostProcess");
    if (pickedPipeline == -1) return;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, postColorTex); // Texture from postProcessFBO
    DrawFullScreenQuad(); 
  }
  else{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, postFBO); 
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glDisable(GL_SCISSOR_TEST);
    glClear(GL_COLOR_BUFFER_BIT); 
    glBlitFramebuffer(
      0, 0, RSwidth, RSheight,
      0, 0, RSwidth, RSheight, 
      GL_COLOR_BUFFER_BIT,
      GL_NEAREST
    );
  }

  
  SDL_GL_SwapWindow(window);
  ClearQueues();

} 


void RenderServer::RenderQueue(const std::vector<RenderInstance>& queue) {
  if (queue.empty()) return;
  GLuint currentVAO = queue[0].vaoID == 0 ? gVertexArrayObject : queue[0].vaoID;
  GLuint currentTexture = queue[0].textureID;
  GLuint currentIndexCount = queue[0].indexCount;
  // collect 
  std::vector<glm::mat4> batchMatrices;
  
  // bind 2d VAO here
  glBindVertexArray(gVertexArrayObject);
  std::cout<<"hit renderQueue: sizeof "<<queue.size()<<std::endl;
  for (size_t i = 0; i < queue.size(); ++i) {
    const auto& instance = queue[i];
    GLuint instanceVAO = instance.vaoID == 0 ? gVertexArrayObject : instance.vaoID;
    if (instanceVAO != currentVAO || instance.textureID != currentTexture) {
            FlushInstancedBatch(currentVAO, currentTexture, currentIndexCount, batchMatrices);
            
            currentVAO = instanceVAO;
            currentTexture = instance.textureID;
            currentIndexCount = instance.indexCount;
            batchMatrices.clear();
        }
        batchMatrices.push_back(instance.globalTransform);
    batchMatrices.push_back(instance.globalTransform);
    // batchMatrices.push_back(instance.globalTransform);
  }
  FlushInstancedBatch(currentVAO, currentTexture, currentIndexCount, batchMatrices);
  // FlushInstancedBatch2d(currentTexture, batchMatrices);
  //FlushInstancedBatch(currentTexture, batchMatrices);
  glBindVertexArray(0);
}


void RenderServer::FlushInstancedBatch2d(GLuint textureID, const std::vector<glm::mat4>& matrices){
  
  FlushInstancedBatch(gVertexArrayObject, textureID, 6, matrices);
}
#include <glm/gtx/io.hpp>
void RenderServer::FlushInstancedBatch(GLuint vaoID, GLuint textureID, GLuint indexCount, const std::vector<glm::mat4>& matrices) {
  std::cout<<"    flushing batch: id:"<<vaoID<<" textureID:"<<textureID<<" indexCount:"<<indexCount<<" some of matrix:";
    for (const auto& mat : matrices) {
        std::cout << mat << std::endl;
    }
  glBindVertexArray(vaoID); // Now it's dynamic!
  // texture bind:
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureID);
  
  glBindBuffer(GL_ARRAY_BUFFER, gInstanceVBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, matrices.size() * sizeof(glm::mat4), matrices.data());

  // Draw 'indexCount' instead of a hardcoded 6
  glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr, matrices.size());
  
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
  
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // Direct error feedback
  glDebugMessageCallback(glDebugOutput, nullptr);
  TextureManager::Get().init();
  RSwidth = width;
  RSheight = height;
  VertexSpecification();
  FrameBufferInit();
  InitPipelines();
  return true;
}

void APIENTRY RenderServer::glDebugOutput(GLenum source, 
                            GLenum type, 
                            unsigned int id, 
                            GLenum severity, 
                            GLsizei length, 
                            const char* message, 
                            const void* userParam) 
{
    if (id == 131169 || id == 131185 || id == 131204 || id == 131218) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source) {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;
    
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << "---------------" << std::endl;
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


  // static 1x1 buff for basic 
  std::vector<Vertex> quadVertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, 0.0f},
        {{ 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, 0.0f}, 
        {{ 0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, 0.0f},
        {{-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, 0.0f}
  };
  std::vector<GLuint> quadIndices = {0, 1, 2, 2, 3, 0}; //0 1 2 for tri 1, 2 3 0 for tri2


  glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(Vertex), quadVertices.data(), GL_STATIC_DRAW);

  // notible: marked as dynamic (will be written to often?, preealloc'd big size;
  // glBufferData(GL_ARRAY_BUFFER, MAX_VERTEX_COUNT * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);


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
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadIndices.size() * sizeof(GLuint), quadIndices.data(), GL_STATIC_DRAW);

  // // Vertex Instance Buffer thing
  glGenBuffers(1, &gInstanceVBO);
  glBindBuffer(GL_ARRAY_BUFFER, gInstanceVBO);
  // allocate 
  glBufferData(GL_ARRAY_BUFFER, MAX_SPRITE_COUNT * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);

  std::size_t vec4Size = sizeof(glm::vec4);
  
  for (int i = 0; i < 4; i++) {
      GLuint attribLocation = 4 + i;
      glEnableVertexAttribArray(attribLocation);
      glVertexAttribPointer(attribLocation, 4, GL_FLOAT, GL_FALSE, 
                            sizeof(glm::mat4), (void*)(i * vec4Size));
      glVertexAttribDivisor(attribLocation, 1); 
  }

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
  std::cout<<"just loaded pipeline named: "<<config.name<<std::endl;
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



void RenderServer::DrawFullScreenQuad() {
  static GLuint dummyVAO = 0;

  if (dummyVAO == 0) {
    glGenVertexArrays(1, &dummyVAO);
  }

  glBindVertexArray(dummyVAO);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
}




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

void RenderServer::setProjectionUniform(GLuint programID){
  glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(RSwidth), 0.0f, static_cast<float>(RSheight), -1.0f, 1.0f);
  GLint locProj = glGetUniformLocation(programID, "u_Projection");
  if(locProj != -1) {
      glUniformMatrix4fv(locProj, 1, GL_FALSE, glm::value_ptr(projection));
  }
}
