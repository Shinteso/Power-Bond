//
// Created by Loonj on 4/24/2026.
//
#pragma once

#include "input.h"

class AiInput : public Input {
public:
    void get_input() override;
    void handle_input(World& world, GameObject& obj) override;

    double stand_timer{0.0};
    double stand_cooldown{0.0};
    bool is_standing{false};
};