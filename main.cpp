#include <thread>
#include <iostream>
#include "raylib.h"
#include "src/Sprite.h"
#include "src/Textures.h"
#include "src/Process.h"
#include "src/Tilemap.hpp"
#include "lib/Perlin-Noise.hpp"
#include "src/WorldGenerator.hpp"

constexpr Color CLEAR_COLOR = Color { 0, 0, 0, 255 };
constexpr double UPDATE_DELAY = 0.008;

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;

constexpr Vector2 getVirtualResolution() {
    return Vector2 { 320, 224 };
}

constexpr Vector2 getScreenResolution() {
    return Vector2 { 1280, 800 };
}

const int WorldWidth = 1024;
const int WorldHeight = 1024;

int main()
{
    Vector2 mapOffset = { 0, 0 };
    auto isRunning = std::atomic<bool>(true);
    auto deltaTime = std::atomic<double>(0);
    auto currentTime = std::atomic<double>(0);
    auto isUpdateFinished = std::atomic<bool>(false);

    Vector2 virtualResolution = getVirtualResolution();
    Vector2 screenResolution = getScreenResolution();

    auto time_ui = (unsigned int)( time(nullptr) );
    const siv::PerlinNoise::seed_type seed = time_ui;

    const siv::PerlinNoise perlin{ seed };

    auto time_ui_2 = (unsigned int)( time(nullptr) );
    const siv::PerlinNoise::seed_type seed_2 = time_ui_2 + time_ui;
    const siv::PerlinNoise perlin_2{ seed_2 };

    Process process1;

    InitWindow(screenResolution.x, screenResolution.y, "Zoidar");

    RenderTexture2D canvas = LoadRenderTexture(virtualResolution.x, virtualResolution.y);

    auto textures = std::make_unique<Textures>();
    textures->load("assets/ship.png");
    textures->load("assets/tiles-extended.png");
    textures->load("assets/tiles-obstacles.png");

    auto sprite = std::make_unique<Sprite>(textures->get("assets/ship.png"));
    sprite->move(640, 300);
    SetTargetFPS(60);

    Tilemap tilemap(WorldWidth, WorldHeight, 16, 16);
    Tilemap tilemap_2(WorldWidth, WorldHeight, 16, 16);
    tilemap.setTexture(textures->get("assets/tiles-extended.png"));
    tilemap_2.setTexture(textures->get("assets/tiles-extended.png"));

    int map_x = 0;
    int map_y = 0;
    float map_z = 0.01;

    auto createTileMap = [&]() {
        auto generator = WorldGenerator();
        generator.generate(WorldWidth, WorldHeight);
        auto map = generator.render();

        for (int i = 0; i < map.size(); i++) {
            int x = i % generator.map_width;
            int y = i / generator.map_width;
            tilemap.setTile(x, y, 0, map[i]);
        }
        auto layerData = generator.renderUpperLayer();
        for (int i = 0; i < layerData.size(); i++) {
            int x = i % generator.map_width;
            int y = i / generator.map_width;
            tilemap_2.setTile(x, y, 0, layerData[i]);
        }
    };

    auto createMap = [&](bool regenerate = false){
        createTileMap();
    };

    createMap();

    auto onUpdateMap = [&](double dt){
        int mx = 0;
        int my = 0;
        float scrollSpeed = 128.0f * (float)dt;
        bool updateNeeded = false;
        float mz = map_z;
        if (IsKeyDown(KEY_RIGHT)) {
            mapOffset.x += scrollSpeed;
        };
        if (IsKeyDown(KEY_LEFT)) {
            mapOffset.x -= scrollSpeed;
        }
        if (IsKeyDown(KEY_UP)) {
            mapOffset.y -= scrollSpeed;
        }
        if (IsKeyDown(KEY_DOWN)) {
            mapOffset.y += scrollSpeed;
        }

        if (IsKeyDown(KEY_N)) {
            createMap();
        }
        if (IsKeyDown(KEY_M)) {
            createMap(true);
        }
        if (IsKeyDown(KEY_I)) {
            // wfcMap.invalidateMap();
        }
        if (IsKeyDown(KEY_R)) {
            createMap();
        }

        if (mapOffset.x < 0) mapOffset.x = 0;
        if (mapOffset.y < 0) mapOffset.y = 0;


        return true;
    };

    process1.addProcess(onUpdateMap);

    auto onUpdate = [&](){
        while (isRunning) {
            double now = GetTime();
            deltaTime = now - currentTime;
            if (deltaTime < UPDATE_DELAY) {
                auto delayDelta = (UPDATE_DELAY - deltaTime) * 1000;
                auto delay = std::chrono::milliseconds ((int) delayDelta);
                std::this_thread::sleep_for(delay);
            }
            currentTime = now;
            process1.onUpdate(deltaTime);
        }
        std::cout << "Update thread finished\n";
        isUpdateFinished = true;
    };

    std::thread updateThread(onUpdate);
    updateThread.detach();

    const Rectangle canvasSourceRect = (Rectangle) { 0, 0, (float)canvas.texture.width, (float)-canvas.texture.height };
    const Rectangle canvasDestRect = Rectangle { 0, 0, 1280, 800 };
    while (!WindowShouldClose())
    {
        BeginTextureMode(canvas);
        ClearBackground(CLEAR_COLOR);
        tilemap.draw(-mapOffset.x, -mapOffset.y, virtualResolution.x, virtualResolution.y);
        tilemap_2.draw(-mapOffset.x, -mapOffset.y, virtualResolution.x, virtualResolution.y);
        sprite->draw();
        EndTextureMode();

        BeginDrawing();
        DrawTexturePro(canvas.texture, canvasSourceRect, canvasDestRect, Vector2 { 0, 0 }, 0, WHITE);
        EndDrawing();
    }

    isRunning = false;

    while(!isUpdateFinished) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    CloseWindow();

    return 0;
}