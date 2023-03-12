#include <thread>
#include <iostream>
#include "raylib.h"
#include "src/Sprite.h"
#include "src/Textures.h"
#include "src/Process.h"

constexpr Color CLEAR_COLOR = Color { 0, 0, 0, 255 };
constexpr double UPDATE_DELAY = 0.008;

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;

int main(void)
{
    auto isRunning = std::atomic<bool>(true);
    auto deltaTime = std::atomic<double>(0);
    auto currentTime = std::atomic<double>(0);

    Process process1;

    InitWindow(1280, 600, "Zoidar");

    auto textures = std::make_unique<Textures>();
    textures->load("assets/ship.png");

    auto sprite = std::make_unique<Sprite>(textures->get("assets/ship.png"));
    sprite->move(640, 300);
    SetTargetFPS(60);

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

    process1.addProcess(onUpdateGhost);
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
    };

    std::thread updateThread(onUpdate);
    updateThread.detach();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(CLEAR_COLOR);
        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        sprite->draw();
        EndDrawing();
    }

    isRunning = false;
    CloseWindow();

    return 0;
}