//
// Created by LJ on 2/12/2026.
//
#include "game_object.h"
#include "physics.h"
#include "world.h"
#include "fsm.h"
#include "action.h"
#include "input.h"

GameObject::GameObject(std::string name, FSM* fsm, Input* input, const Color& color)
    : obj_name{name}, fsm{fsm}, input{input}, color{color} {}

GameObject::~GameObject() {
    delete fsm;
    delete input;
}

void GameObject::update(World &world, double dt) {
    if (fsm != nullptr) {
        fsm->current_state->update(world, *this, dt);
    }
    sprites[sprite_name].update(dt);
    set_sprite(sprite_name);
    if (iframe_time_remaining > 0.0) iframe_time_remaining -= dt;
}

std::pair<Vec<float>, Color> GameObject::get_sprite() const {
    return {physics.position, color};
}

void GameObject::set_sprite(const std::string& next_sprite) {
    // if the next sprite is a new sprite, reset current sprite
    if (next_sprite != sprite_name) {
        sprites[sprite_name].reset();

        auto itr = sprites.find(next_sprite);
        if (itr != sprites.end()) {
            sprite_name = next_sprite;
        }
        else {
            sprite_name = "idle"; // assume every GameObject has an idle sprite
        }
    }
    sprite = sprites[sprite_name].get_sprite();
}

AABB GameObject::get_bounding_box() {
    Vec<float> half_size = {size.x / 2.0f, size.y / 2.0f};
    Vec<float> center = {physics.position.x + half_size.x, physics.position.y + half_size.y};
    AABB bounding_box{center, half_size};
    return bounding_box;
}

void GameObject::take_damage(int attack_damage) {
    if (iframe_time_remaining > 0) return;
    health -= attack_damage;
    iframe_time_remaining = 1;
    if (health <= 0) {
        is_alive = false;
    }
}

bool GameObject::flash_sprite() const {
    if (iframe_time_remaining <= 0.0) return false;
    bool result = ((SDL_GetTicks() / 80) % 2) == 0;
    return result;
}

