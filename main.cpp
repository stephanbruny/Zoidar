#include <thread>
#include <iostream>
#include "raylib.h"
#include "src/Sprite.h"
#include "src/Textures.h"
#include "src/Process.h"
#include "src/Tilemap.hpp"
#include "lib/Perlin-Noise.hpp"

constexpr Color CLEAR_COLOR = Color { 0, 0, 0, 255 };
constexpr double UPDATE_DELAY = 0.008;

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;

int main(void)
{
    auto isRunning = std::atomic<bool>(true);
    auto deltaTime = std::atomic<double>(0);
    auto currentTime = std::atomic<double>(0);
    auto isUpdateFinished = std::atomic<bool>(false);

    unsigned int time_ui = (unsigned int)( time(NULL) );
    const siv::PerlinNoise::seed_type seed = time_ui;

    const siv::PerlinNoise perlin{ seed };

    unsigned int time_ui_2 = (unsigned int)( time(NULL) );
    const siv::PerlinNoise::seed_type seed_2 = time_ui_2 + time_ui;
    const siv::PerlinNoise perlin_2{ seed_2 };

    Process process1;

    InitWindow(1280, 600, "Zoidar");

    auto textures = std::make_unique<Textures>();
    textures->load("assets/ship.png");
    textures->load("assets/tiles.png");

    auto sprite = std::make_unique<Sprite>(textures->get("assets/ship.png"));
    sprite->move(640, 300);
    SetTargetFPS(60);

    Tilemap tilemap(80, 38, 16, 16);
    Tilemap tilemap_2(80, 38, 16, 16);
    tilemap.setTexture(textures->get("assets/tiles.png"));
    tilemap_2.setTexture(textures->get("assets/tiles.png"));

    tilemap_2.setMinIndex(14);

    int map_x = 0;
    int map_y = 0;
    float map_z = 0.01;

    auto createMap = [&](){
        for (int x = 0; x < tilemap.size.width; x++) {
            for (int y = 0; y < tilemap.size.height; y++) {
                const double noise = perlin.noise2D(((x + map_x) * map_z), ((y + map_y) * map_z)) * 10;
                tilemap.setTile(x, y, 0, (int)noise);
            }
        }
    };

    auto createUpperLayerMap = [&](){
        for (int x = 0; x < tilemap_2.size.width; x++) {
            for (int y = 0; y < tilemap_2.size.height; y++) {
                const int lower_index = tilemap.getTile(x, y).index;
                if (lower_index > 3) {
                    const double noise = perlin_2.noise2D_01(((x + map_x) * 0.1), ((y + map_y) * 0.1)) * 20;
                    const int n = (int)noise;
                    tilemap_2.setTile(x, y, 0, n);
                } else {
                    tilemap_2.setTile(x, y, 0, 0);
                }
            }
        }
    };

    createMap();
    createUpperLayerMap();

    auto onUpdateMap = [&](double dt){
        int mx = 0;
        int my = 0;
        float mz = map_z;
        if (IsKeyDown(KEY_RIGHT)) mx = 1;
        if (IsKeyDown(KEY_LEFT)) mx = -1;
        if (IsKeyDown(KEY_UP)) my = -1;
        if (IsKeyDown(KEY_DOWN)) my = 1;
        if (IsKeyDown(KEY_Q)) mz /= 1.001;
        if (IsKeyDown(KEY_A)) mz *= 1.001;
        if (mx != 0 || my != 0 || mz != 0) {
            map_x += mx;
            map_y += my;
            map_z = mz;
            createMap();
            createUpperLayerMap();
        }
        return true;
    };

    auto onUpdateGhost = [&](double dt){
        if (IsKeyDown(KEY_RIGHT)) sprite->moveRel(1, 0);
        if (IsKeyDown(KEY_LEFT)) sprite->moveRel(-1, 0);
        if (IsKeyDown(KEY_UP)) sprite->moveRel(0, -1);
        if (IsKeyDown(KEY_DOWN)) sprite->moveRel(0, 1);
        return true;
    };

    auto onUpdateGhostReset = [&](double dt) {
        if (sprite->position.x > 1280) {
            sprite->move(1280, sprite->position.y);
        }
        if (sprite->position.x < 0) {
            sprite->move(0, sprite->position.y);
        }
        return true;
    };

    process1.addProcess(onUpdateMap);
    process1.addProcess(onUpdateGhostReset);

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

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(CLEAR_COLOR);
        tilemap.draw();
        tilemap_2.draw();
        sprite->draw();
        EndDrawing();
    }

    isRunning = false;

    while(!isUpdateFinished) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    textures->clear();
    CloseWindow();

    return 0;
}