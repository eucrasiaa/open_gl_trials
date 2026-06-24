#include "Engine.hpp"

SDL_Window* Engine::window = nullptr;
SDL_GLContext Engine::glContext = nullptr;
bool Engine::isRunning = false;
double Engine::timeScale = 1.0;
std::vector<Node*> Engine::sceneNodes;

bool Engine::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL Init Error: %s\n", SDL_GetError());
        return false;
    }



    // // Set core OpenGL context parameters before window generation
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    //
    // Mandate the SDL_WINDOW_OPENGL frame allocation flag
    window = SDL_CreateWindow(title, 
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                              width, height, 
                              SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window Error: %s\n", SDL_GetError());
        return false;
    }

    // Bind the hardware OpenGL context instance to the window handle
    

    glContext = SDL_GL_CreateContext(window);




    if (!glContext) {
        printf("OpenGL Context Error: %s\n", SDL_GetError());
        return false;
    }

    // Synchronize frame swap to monitor vertical blank refresh 
    SDL_GL_SetSwapInterval(1); 

    isRunning = true;
    return true;
}

void Engine::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
            shutdown();
        }
        else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_LEFT:
                    changeResolution(-1); // Cycle down
                    break;
                case SDLK_RIGHT:
                    changeResolution(1);  // Cycle up
                    break;
            }
        }
    }
}

void Engine::changeResolution(int direction) {
    int newIndex = currentResIndex + direction;
    
    // Bounds check to keep within our fixed array size (0 to 3)
    if (newIndex >= 0 && newIndex < 4) {
        currentResIndex = newIndex;
        Resolution target = resPool[currentResIndex];
        
        // Tells the OS windowing system to instantly snap window dimensions
        SDL_SetWindowSize(window, target.width, target.height);
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        
        printf("Resolution changed to: %dx%d\n", target.width, target.height);
    }
}


void Engine::update(double dt) {
    if (sceneNodes.size() > 1) {
        Node* player = sceneNodes[1];
        
        float speed = 150.0f; // Pixels per second
        player->position.x += speed * static_cast<float>(dt);
    }
}

void Engine::render() {
    // glClear(GL_COLOR_BUFFER_BIT);

    // 2. Loop through every active engine spatial node
    for (auto* node : sceneNodes) {
        if (node != nullptr) {
            node->render(); // Dispatches internal RenderElems
        }
    }
}

void Engine::run() {
    double accumulator = 0.0;
    const double FIXED_DT = 1.0 / 60.0; 
    const double MAX_DELTA = 0.25;

    uint64_t frequency = SDL_GetPerformanceFrequency();
    uint64_t currentTicks = SDL_GetPerformanceCounter();

    while (isRunning) {
        uint64_t newTicks = SDL_GetPerformanceCounter();
        double frameTime = static_cast<double>(newTicks - currentTicks) / frequency;
        currentTicks = newTicks;

        if (frameTime > MAX_DELTA) {
            frameTime = MAX_DELTA;
        }

        accumulator += (frameTime * timeScale);

        handleEvents();

        while (accumulator >= FIXED_DT) {
            update(FIXED_DT);
            accumulator -= FIXED_DT;
        }

        render();

        // Swap back-buffer to screen front-buffer frame pipeline
        SDL_GL_SwapWindow(window);
    }
}
void Engine::addNode(Node* node) {
    if (node != nullptr) {
        sceneNodes.push_back(node);
    }
}

void Engine::shutdown() {
    for (auto* node : sceneNodes) {
        delete node;
    }
    sceneNodes.clear();

    if (glContext) SDL_GL_DeleteContext(glContext);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
    if (glContext) SDL_GL_DeleteContext(glContext);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}
const Engine::Resolution Engine::resPool[4] = {
    { 640,  360 },
    { 1280, 720 },
    { 1600, 900 },
    { 1920, 1080 }
};
int Engine::currentResIndex = 1;

