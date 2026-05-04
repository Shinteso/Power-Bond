//
// Created by Loonj on 2/13/2026.
//
#include "action.h"
#include "game_object.h"
#include "states.h"
#include "world.h"

void Jump::perform(World& world, GameObject &obj) {
    obj.physics.velocity.y = obj.physics.jump_velocity;
    world.audio->play_sounds("jump");
}

void WalkRight::perform(World &, GameObject &obj) {
    obj.physics.acceleration.x = obj.physics.walk_acceleration;
    obj.facing_right = true;
}

void WalkLeft::perform(World &, GameObject& obj) {
    obj.physics.acceleration.x = -obj.physics.walk_acceleration;
    obj.facing_right = false;
}

void RunRight::perform(World &, GameObject& obj) {
    obj.physics.acceleration.x = obj.physics.walk_acceleration * 1.5f;
    obj.facing_right = true;
}
void RunLeft::perform(World &, GameObject& obj) {
    obj.physics.acceleration.x = -obj.physics.walk_acceleration * 1.5f;
    obj.facing_right = false;
}

void ShootBullet::perform(World& world, GameObject& obj) {
    auto bullet = dynamic_cast<Projectile*>(world.available_items["bullet"]());
    bullet->sprites["idle"].flip(!obj.facing_right);

    // determine direction
    float direction = obj.facing_right ? 1.0f : -1.0f;

    // set sprite based on current state
    if (obj.fsm->current_state_type == StateType::Walking) {
        obj.fsm->transition(Transition::AttackingWalking, world, obj);
    } else if (obj.fsm->current_state_type == StateType::Running) {
        obj.fsm->transition(Transition::AttackingRunning, world, obj);
    } else {
        obj.fsm->transition(Transition::Attacking, world, obj);
    }

    // spawn at gun position using shift and positioning
    bullet->physics.position = {obj.physics.position.x + (direction > 0 ? obj.size.x : 0.0f), obj.physics.position.y}; // roughly gun height with shift
    bullet->physics.velocity.x = direction * 15.0f;
    bullet->facing_right = obj.facing_right;
    world.projectiles.push_back(bullet);

    world.audio->play_sounds("bullet");
}

