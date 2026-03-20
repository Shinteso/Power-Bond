//
// Created by Loonj on 2/18/2026.
//
#include "states.h"
#include "action.h"

// Helper Function
bool on_platform(World& world, GameObject& obj) {
    constexpr float epsilon = 1e-4;
    Vec<float> left_foot{obj.physics.position.x + epsilon,obj.physics.position.y - epsilon};
    Vec<float> right_foot{obj.physics.position.x + obj.size.x -epsilon,obj.physics.position.y - epsilon};
    return world.collides(left_foot) || world.collides(right_foot);
}

// Standing
void Standing::on_enter(World&, GameObject& obj) {
    obj.color = {255, 0, 0, 255};
    obj.set_sprite("idle");
    obj.physics.acceleration.x = 0;
}

Action* Standing::input(World& world, GameObject& obj, ActionType action_type) {
    if (action_type == ActionType::Jump) {
        obj.fsm->transition(Transition::Jump, world, obj);
        return new Jump();
    }
    else if  (action_type == ActionType::MoveRight) {
        obj.fsm->transition(Transition::Move, world, obj);
        return new MoveRight();
    }
    else if (action_type == ActionType::MoveLeft) {
        obj.fsm->transition(Transition::Move, world, obj);
        return new MoveLeft();
    }
    if (action_type == ActionType::StageTransition) {
        obj.fsm->transition(Transition::StageTransition, world, obj);
    }
    if (action_type == ActionType::Attacking) {
    obj.fsm->transition(Transition::Attacking, world, obj);
    }
    return nullptr;
}

// InAir
void InAir::on_enter(World&, GameObject& obj) {
    elapsed = cooldown;
    obj.color = {0, 0, 255};
}

void InAir::update(World& world, GameObject& obj, double dt) {
    elapsed -= dt;
    if (elapsed <= 0 && on_platform(world, obj)) {
        obj.fsm->transition(Transition::Stop, world, obj);
    }
}

Action *InAir::input(World&, GameObject& obj, ActionType action_type) {
    if (action_type == ActionType::MoveLeft) {
        return new MoveLeft();
    }
    else if (action_type == ActionType::MoveRight) {
        return new MoveRight();
    }
    return nullptr;
}

// Running
void Running::on_enter(World&, GameObject& obj) {
    obj.color = {255, 255, 0, 255};
    obj.set_sprite("walking");
}

Action* Running::input(World& world, GameObject& obj, ActionType action_type) {
    if (action_type == ActionType::None) {
        obj.fsm->transition(Transition::Stop, world, obj);
    }
    else if (action_type == ActionType::Jump) {
        obj.fsm->transition(Transition::Jump, world, obj);
        return new Jump();
    }
    else if (action_type == ActionType::MoveRight) {
        obj.fsm->transition(Transition::Move, world, obj);
        return new MoveRight();
    }
    else if (action_type == ActionType::MoveLeft) {
        obj.fsm->transition(Transition::Move, world, obj);
        return new MoveLeft();
    }
    return nullptr;
}

void StageTransition::on_enter(World&, GameObject& obj) {
    obj.color = {155, 155, 100, 255};
}

Action* StageTransition::input(World& world, GameObject& obj, ActionType action_type) {
    if (action_type == ActionType::None) {
        obj.fsm->transition(Transition::Stop, world, obj);
    }
    return nullptr;
}


void Attacking::on_enter(World&, GameObject& obj) {
    obj.color = {200, 100, 155};
}
Action* Attacking::input(World& world, GameObject& obj, ActionType action_type) {
    if (action_type == ActionType::None) {
        obj.fsm->transition(Transition::Stop, world, obj);
    }
    return nullptr;
}

