#include "PlayerElem.hpp"
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

    if (!Engine::Get().init("OpenGL Engine Layout", 1280, 720)) {
        return 1;
    }

    RenderElem* background = new RenderElem();
    background->init("../assets/textures/pleasentgradient.png", RenderItemLayer::OPAQUE);
    background->position = Vec2(640.0f, 360.0f); 
    background->scale = Vec2(1280.0f, 720.0f);
    Engine::Get().addNode(background);


    PlayerElem* player = new PlayerElem();
    player->init("../assets/textures/player.png", RenderItemLayer::OPAQUE);
    player->position = Vec2(1280.0/2.0, 720.0/2.0);
    player->scale = Vec2(100.0f, 100.0f);
    Engine::Get().addNode(player);

    Engine::Get().run();
    Engine::Get().shutdown();
    return 0;
}
