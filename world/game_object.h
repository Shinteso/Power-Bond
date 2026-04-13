//
// Created by LJ on 2/12/2026.
//
#pragma once
#include <utility>
#include <map>
#include "vec.h"
#include "graphics.h"
#include "physics.h"
#include "animated_sprite.h"

class World;
class FSM;
class Input;

using Sprites = std::map<std::string, AnimatedSprite>;

class GameObject {
public:
    GameObject(const Vec<int>& size, FSM* fsm, Input* input, Color color);
    ~GameObject();


    void update(World& world, double dt);


    std::pair<Vec<float>, Color> get_sprite() const;
    void set_sprite(const std::string& next_sprite);


    // Player data
    Physics physics;
    Vec<int> size;
    FSM* fsm;
    Input* input;
    Color color;
    Sprites sprites;
    Sprite sprite;
    std::string sprite_name ="idle";
};