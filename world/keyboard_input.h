//
// Created by Loonj on 2/25/2026.
//
#pragma once
#include "input.h"

class KeyboardInput : public Input {
public:
    void get_input() override;
    void handle_input(World& world, GameObject& obj) override;
    Action* collect_discrete_event(SDL_Event* event) override;

    bool shoot_pending{false};
    double shoot_cooldown{4.0};
    double shoot_rate{4.0}; // seconds between shots
};
