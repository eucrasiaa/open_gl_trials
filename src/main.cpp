#define SDL_MAIN_HANDLED
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "Engine.hpp"

#include "SpriteElement.hpp"



int main(int argc, char* argv[]) {
    // Starts at index 1 (1280x720) out of our resolution pool
    if (!Engine::init("OpenGL Engine Layout", 1280, 720)) {
        return 1;
    }
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
