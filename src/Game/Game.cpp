#include "Game.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include <memory>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

Game::Game() {
    isRunning = false;
    registry = std::make_unique<Registry>();
    assetStore = std::make_unique<AssetStore>();
    Logger::Log("Game constructor called!");
}

Game::~Game() {
    Logger::Log("Game destructor called!");
}

void Game::Initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        Logger::Err("Error initializing SDL.");
        return;
    }
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    windowWidth = displayMode.w;
    windowHeight = displayMode.h;
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_BORDERLESS
    );
    if (!window) {
        Logger::Err("Error creating SDL window.");
        return;
    }
    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer) {
        Logger::Err("Error creating SDL renderer.");
        return;
    }
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    isRunning = true;
}

void Game::ProcessInput() {
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
        switch (sdlEvent.type) {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_KEYDOWN:
                if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
                    isRunning = false;
                }
                break;
        }
    }
}

void Game::LoadLevel(int level) {
    // Add the systems that need to be processed in our game
    registry->AddSystem<MovementSystem>();
    registry->AddSystem<RenderSystem>();

    // Adding assets to the asset store
    assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
    assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
    assetStore->AddTexture(renderer, "tilemap-image", "./assets/tilemaps/jungle.png");

    // Load the tilemap
    //int mapTileX = 0;
    //int mapTileY = 0;
    //std::fstream mapFile;
    //mapFile.open("./assets/tilemaps/jungle.map", std::ios::in);
    //if(mapFile.is_open()) {
    //    std::string mapLine;
    //    while (getline(mapFile, mapLine)) {
    //        mapTileX = 0;
    //        std::stringstream mapLineStream(mapLine);
    //        std::string mapTileIdString;
    //        while (getline(mapLineStream, mapTileIdString, ',')) {
    //            Entity tile = registry->CreateEntity();
    //            int mapTileId = std::stoi(mapTileIdString);
    //            int mapTileOffsetX = (mapTileId % 10) * 32;
    //            int mapTileOffsetY = static_cast<int>(floor(mapTileId / 10) * 32);
    //            //Logger::Log("Map tile id = " + std::to_string(mapTileId) + " (" + std::to_string(mapTileOffsetX) + ", " + std::to_string(mapTileOffsetY) + ") at position (" + std::to_string(mapTileX * 32) + ", " + std::to_string(mapTileY * 32) + ")");
    //            tile.AddComponent<TransformComponent>(glm::vec2(mapTileX * 32, mapTileY * 32), glm::vec2(1.0, 1.0), 0.0);
    //            tile.AddComponent<SpriteComponent>("jungle-image", 32, 32, mapTileOffsetX, mapTileOffsetY);
    //            mapTileX++;
    //        }
    //        mapTileY++;
    //    }
    //    mapFile.close();
    //}

    // Load the tilemap
    int tileSize = 32;
    double tileScale = 2.0;
    int mapNumCols = 25;
    int mapNumRows = 20;
    
    std::fstream mapFile;
    mapFile.open("./assets/tilemaps/jungle.map");

    for (int y = 0; y < mapNumRows; y++) {
        for (int x = 0; x < mapNumCols; x++) {
            char ch;
            mapFile.get(ch);
            int srcRectY = std::atoi(&ch) * tileSize;
            mapFile.get(ch);
            int srcRectX = std::atoi(&ch) * tileSize;
            mapFile.ignore();

            Entity tile = registry->CreateEntity();
            tile.AddComponent<TransformComponent>(glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)), glm::vec2(tileScale, tileScale), 0.0);
            tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, srcRectX, srcRectY);
        }
    }

    // Create an entity
    Entity tank = registry->CreateEntity();
    tank.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(40.0, 0.0));
    tank.AddComponent<SpriteComponent>("tank-image", 32, 32);
    
    // Create an entity
    Entity truck = registry->CreateEntity();
    truck.AddComponent<TransformComponent>(glm::vec2(50.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 50.0));
    truck.AddComponent<SpriteComponent>("truck-image", 32, 32);
}

void Game::Setup() {
    LoadLevel(1);
}

void Game::Update() {
    // Cap the framerate at desired maximum
    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
    if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME) {
        SDL_Delay(timeToWait);
    }

    // The difference in ticks since the last frame, converted to seconds
    double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;

    // Store the "previous" frame time
    millisecsPreviousFrame = SDL_GetTicks();

    // Update the registry to precess the entities that are waiting to be created/deleted
    registry->Update();

    // Invoke all the systems that need to update 
    registry->GetSystem<MovementSystem>().Update(deltaTime);
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);

    // Invoke all the systems that need to render
    registry->GetSystem<RenderSystem>().Update(renderer, assetStore);

    SDL_RenderPresent(renderer);
}

void Game::Run() {
    Setup();
    while (isRunning) {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::Destroy() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
