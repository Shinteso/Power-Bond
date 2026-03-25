//
// Created by Loonj on 1/28/2026.
//
#pragma once
#include "world.h"
#include "camera.h"
#include "game_object.h"
#include "graphics.h"
#include "audio.h"

class Game {
public:
    Game(std::string title, int width, int height);
    void handle_event(SDL_Event* event);
    void input();
    void update();
    void render();

private:
    Graphics graphics;
    std::unique_ptr<GameObject> player;
    World* world;
    Camera camera;
    Audio audio;

    // timing
    float dt;
    Uint64 performance_frequency;
    Uint64 prev_counter;
    float lag;
};
