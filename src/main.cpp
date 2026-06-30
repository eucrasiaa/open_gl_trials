#include "RenderItem.hpp"
#include "SceneElems.hpp"
#define SDL_MAIN_HANDLED
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "Engine.hpp"

#include "SpriteElement.hpp"



int main(int argc, char* argv[]) {
      SDL_SetMainReady();
    // Starts at index 1 (1280x720) out of our resolution pool
    if (!Engine::Get().init("OpenGL Engine Layout", 1280, 720)) {
        return 1;
    }
    Node* background = new Node();
    background->position = Vec2::ZERO;
    background->scale = Vec2(1280.0f, 720.0f);
    RenderElem *bkgd = new RenderElem();
    bkgd->init("../assets/textures/pleasentgradient.png",RenderItemLayer::OPAQUE);
    background->children.push_back(bkgd);
    
    Engine::Get().addNode(background);
    Node* player = new Node();
    player->position = Vec2(100.0f, 200.0f);
    player->scale = Vec2(32.0f, 62.0f);
    bkgd = new RenderElem();
    bkgd->init("../assets/textures/player.png",RenderItemLayer::OPAQUE);
    player->children.push_back(bkgd);

    Engine::Get().addNode(player);
  

    Engine::Get().run();
    Engine::Get().shutdown();

    return 0;
}
