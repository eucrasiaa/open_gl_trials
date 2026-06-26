#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "Engine.hpp"

class SpriteElement : public RenderElem {
public:
  //open gl here

};


// VAO
GLuint gVertexArrayObject  = 0;
// VBO
GLuint gVertexBufferObject = 0;

const std::string gVertexShaderSource = R"glsl(
  #version 460 core
  in vec4 position;
  uniform float u_Time;
  void main ()
  {
    float shiftedX = position.x + sin(u_Time) * 0.5;
    gl_Position = vec4(shiftedX, position.y, position.z, position.w);
  }

)glsl";

const std::string gFragmentShaderSource = R"glsl(
  #version 460 core
  out vec4 color;
  void main ()
  {
    color = vec4(1.0f, 0.5f, 0.0f, 1.0f);
  }

)glsl";


void VertexSpecification(){
  // to the CPU
  const std::vector<GLfloat> vertexPosition{
  //  x      y     z
    -0.8f, -0.8f, 0.0f,  //v1
     0.8f, -0.8f, 0.0f,  //v2
     0.0f,  0.8f, 0.0f,  //v3
  };
  // to the GPU
  //create a VAO
  glGenVertexArrays(1,&gVertexArrayObject);
  //select VAO
  glBindVertexArray(gVertexArrayObject);

  //VBO
  // gen the buffer  VBO
  glGenBuffers(1, &gVertexBufferObject);
  // select the buffer
  glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);

  //populate the VBO
  //              target,            size(bytes),           *data: .data() gives raw ptr,              Usage
  glBufferData(GL_ARRAY_BUFFER, vertexPosition.size()*sizeof(GLfloat), vertexPosition.data(), GL_STATIC_DRAW);

  // now back to VAO
  //   we enable the 1 attrib we have, pos!
  glEnableVertexAttribArray(0);

  //  telling the gpu the info of our VBO defined in the VAO? 
  glVertexAttribPointer(0, 3, 
                        GL_FLOAT, GL_FALSE, 
                        0, (void*)0
                        );

  // cleanup. just undo the bindings?
  glBindVertexArray(0);
  glDisableVertexAttribArray(0);
}

// program object for our shaders
//  handle to the pipeline that we compile, holding vertex and fragment shader
GLuint gGraphicsShaderPipelineProgram = 0;

GLuint CompileShader(GLuint type, const std::string& source){
  //oh boy 
  GLuint shaderObject;
  if(type==GL_VERTEX_SHADER){
    shaderObject = glCreateShader(GL_VERTEX_SHADER);
  
  }else if (type==GL_FRAGMENT_SHADER){
    shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
  }
  const char* str = source.c_str(); // its a C library so.
  glShaderSource(shaderObject,1,&str,nullptr);
  glCompileShader(shaderObject);
  return shaderObject;
}

// rets: handle to gpu shader program. so many ints
GLuint CreateShaderProgram(const std::string& vertShaderSrc, const std::string& fragShaderSrc){
  GLuint programObject = glCreateProgram(); // creates empty, we'll fill in. think of it like just creating pipeline
  
  GLuint myVertexShader = CompileShader(GL_VERTEX_SHADER, vertShaderSrc);
  GLuint myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragShaderSrc);
  // takes results, assemble in some way into the pipeline

  glAttachShader(programObject, myVertexShader);
  glAttachShader(programObject, myFragmentShader);
  // link in together
  glLinkProgram(programObject);
  // validate?
  glValidateProgram(programObject);

  // cleanup
  // tbd
  // glDetachShader(programObject, myVertexShader);
  // glDeleteShader(myVertexShader);
  //
  // glDetachShader(programObject, myFragmentShader);
  // glDeleteShader(myFragmentShader);
  return programObject;
}
void CreateGraphicsPipeline(){
   
  gGraphicsShaderPipelineProgram = CreateShaderProgram(gVertexShaderSource,gFragmentShaderSource);

}

void Engine::render(double ft) {
  
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);

  glUseProgram(gGraphicsShaderPipelineProgram);

  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
 
  GLint locToPass = glGetUniformLocation(gGraphicsShaderPipelineProgram, "u_Time");
  glUniform1f(locToPass, static_cast<float>(ft));
      glBindVertexArray(gVertexArrayObject);
 
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);

  glDrawArrays(GL_TRIANGLES, 0, 3);

  for (auto* node : sceneNodes) {
    if (node != nullptr) {
      node->render(); // each node will share their rendering 
    }
  }
}

int main(int argc, char* argv[]) {
    // Starts at index 1 (1280x720) out of our resolution pool
    if (!Engine::init("OpenGL Engine Layout", 1280, 720)) {
        return 1;
    }


    VertexSpecification();
    CreateGraphicsPipeline();


    std::cout<<gVertexArrayObject<< " " << gVertexBufferObject << " " << gGraphicsShaderPipelineProgram << std::endl;
    Node* background = new Node();
    background->position = Vec2::ZERO;
    background->scale = Vec2(1280.0f, 720.0f);
    background->renderElements.push_back(new SpriteElement());
    
    Engine::addNode(background);
    Node* player = new Node();
    player->position = Vec2(100.0f, 200.0f);
    player->scale = Vec2(32.0f, 62.0f);
    player->renderElements.push_back(new SpriteElement());

    Engine::addNode(player);
  

    Engine::run();
    Engine::shutdown();

    return 0;
}
