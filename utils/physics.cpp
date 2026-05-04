//
// Created by Loonj on 1/30/2026.
//

#include "physics.h"

Physics::Physics(Vec<float> position, Vec<float> velocity, Vec<float> acceleration)
    : position{position}, velocity{velocity}, acceleration{acceleration} {}

Physics::Physics(Vec<float> position, Vec<float> velocity, Vec<float> acceleration, float gravity, float damping, float walk_acceleration, float jump_velocity, float terminal_velocity)
    : position{position}, velocity{velocity}, acceleration{acceleration}, gravity{gravity}, damping{damping}, walk_acceleration{walk_acceleration}, jump_velocity{jump_velocity}, terminal_velocity{terminal_velocity} {}

