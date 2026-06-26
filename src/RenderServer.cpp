#include "RenderServer.hpp"
#include <iostream>

SDL_Window* RenderServer::window = nullptr;
SDL_GLContext RenderServer::glContext = nullptr;
#include "OldOpenGLHelp.hpp"

void RenderServer::render(double dt){
  // glDisable(GL_DEPTH_TEST);
  // glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  //
  glUseProgram(gGraphicsShaderPipelineProgram);

  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  GLint locToPass = glGetUniformLocation(gGraphicsShaderPipelineProgram, "u_Time");
  glUniform1f(locToPass, static_cast<float>(dt));
  glBindVertexArray(gVertexArrayObject);

  glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);

  glDrawArrays(GL_TRIANGLES, 0, 3);


  SDL_GL_SwapWindow(window);
}




bool RenderServer::init(const char* title, int width, int height){
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr<<"SDL Init Error: %s\n"<<SDL_GetError()<<"\n";
    return false;
  }





    std::cout<<gVertexArrayObject<< " " << gVertexBufferObject << " " << gGraphicsShaderPipelineProgram << std::endl;
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
  

  VertexSpecification();
  CreateGraphicsPipeline();
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
    SDL_ClearError(); // Clear it so it doesn't leak out
    return -1;        // Trigger failure
  }

  // 5. CHECK TARGETED OPENGL ERRORS (Specifically for glViewport)
  GLenum glErr = glGetError();
  if (glErr != GL_NO_ERROR) {
    // glViewport only throws GL_INVALID_VALUE if width or height is negative
    std::cerr << "[Critical] OpenGL viewport assignment failed. Error Code: " << glErr << "\n";
    return -2;        // Trigger failure
  }



  return 0;
}

void RenderServer::shutdown(){
  if (glContext) SDL_GL_DeleteContext(glContext);
  if (window) SDL_DestroyWindow(window);
  SDL_Quit();

}
