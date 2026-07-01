#include "PlayerElem.hpp"
#include "PlayerElem_alt.hpp"
#include "RenderItem.hpp"
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

    RenderNode* background = new RenderNode();
    background->init("../assets/textures/pleasentgradient.png", RenderItemLayer::OPAQUE);
    background->position = Vec2(640.0f, 360.0f); 
    background->scale = Vec2(1280.0f, 720.0f);
    

    Engine::Get().addNode(background);


    PlayerElem* player = new PlayerElem();
    player->init("../assets/textures/player.png", RenderItemLayer::TRANSPARENT);
    // player->position = Vec2(1280.0/2.0, 720.0/2.0);
    
    player->position = Vec2(0.0, 0.0);
    player->scale = Vec2(1.0f, 1.0f);
    
    PlayerElem_alt* player2 = new PlayerElem_alt();
    player2->init("../assets/textures/x.png", RenderItemLayer::TRANSPARENT);
    player2->position = Vec2(1280.0/2.0, 720.0/2.0);
    player2->scale = Vec2(50.0f, 50.0f);
    player2->addChild(player); 

    Engine::Get().addNode(player2);

    Engine::Get().run();
    Engine::Get().shutdown();
    return 0;
}
