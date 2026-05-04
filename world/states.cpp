//
// Created by Loonj on 2/18/2026.
//
#include "states.h"
#include "action.h"
#include "ai_input.h"
#include "game_object.h"
#include "random.h"
#include "world.h"
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
    obj.physics.acceleration.x = 0;
    obj.sprites["idle"].flip(!obj.facing_right);
    obj.set_sprite("idle");
}

Action* Standing::input(World& world, GameObject& obj, ActionType action_type) {
    if (action_type == ActionType::Jump) {
        obj.fsm->transition(Transition::Jump, world, obj);
        return new Jump();
    }
    else if (action_type == ActionType::WalkRight) {
        obj.facing_right = true;
        obj.fsm->transition(Transition::WalkRight, world, obj);
        return new WalkRight();
    }
    else if (action_type == ActionType::WalkLeft) {
        obj.facing_right = false;
        obj.fsm->transition(Transition::WalkLeft, world, obj);
        return new WalkLeft();
    }
    else if (action_type == ActionType::RunRight) {
        obj.facing_right = true;
        obj.fsm->transition(Transition::Move, world, obj);
        return new RunRight();
    }
    else if (action_type == ActionType::RunLeft) {
        obj.facing_right = false;
        obj.fsm->transition(Transition::Move, world, obj);
        return new RunLeft();
    }
    else if (action_type == ActionType::Attacking) {
        obj.fsm->transition(Transition::Attacking, world, obj);
    }
    else if (action_type == ActionType::AttackAll) {
        obj.fsm->transition(Transition::AttackAll, world, obj);
    }
    return nullptr;
}

// InAir
void InAir::on_enter(World&, GameObject& obj) {
    elapsed = cooldown;
    obj.color = {0, 0, 255};
    obj.set_sprite(obj.facing_right ? "jump_right" : "jump_left");
}

void InAir::update(World& world, GameObject& obj, double dt) {
    elapsed -= dt;
    if (elapsed <= 0 && on_platform(world, obj)) {
        obj.fsm->transition(Transition::Stop, world, obj);
    }
}


Action* InAir::input(World&, GameObject& obj, ActionType action_type) {
    if (action_type == ActionType::WalkLeft) {
        obj.facing_right = false;
        return new WalkLeft();
    }
    else if (action_type == ActionType::WalkRight) {
        obj.facing_right = true;
        return new WalkRight();
    }
    else if (action_type == ActionType::RunLeft) {
        obj.facing_right = false;
        return new RunLeft();
    }
    else if (action_type == ActionType::RunRight) {
        obj.facing_right = true;
        return new RunRight();
    }
    return nullptr;
}

// walking
void Walking::on_enter(World&, GameObject& obj) {
    obj.color = {255, 155, 0, 255};
    obj.set_sprite(obj.facing_right ? "walking_right" : "walking_left");
}


Action* Walking::input(World& world, GameObject& obj, ActionType action_type) {
    if (action_type == ActionType::None) {
        obj.fsm->transition(Transition::Stop, world, obj);
    }
    else if (action_type == ActionType::Jump) {
        obj.fsm->transition(Transition::Jump, world, obj);
        return new Jump();
    }
    else if (action_type == ActionType::WalkRight) {
        obj.facing_right = true;
        obj.fsm->transition(Transition::WalkRight, world, obj);
        return new WalkRight();
    }
    else if (action_type == ActionType::WalkLeft) {
        obj.facing_right = false;
        obj.fsm->transition(Transition::WalkLeft, world, obj);
        return new WalkLeft();
    }
    else if (action_type == ActionType::RunRight) {
        obj.facing_right = true;
        obj.fsm->transition(Transition::Move, world, obj);
        return new RunRight();
    }
    else if (action_type == ActionType::RunLeft) {
        obj.facing_right = false;
        obj.fsm->transition(Transition::Move, world, obj);
        return new RunLeft();
    }
    else if (action_type == ActionType::AttackWalking) {
        obj.fsm->transition(Transition::AttackingWalking, world, obj);
    }
    return nullptr;
}


// Running
void Running::on_enter(World&, GameObject& obj) {
    obj.color = {255, 255, 0, 255};
    obj.set_sprite(obj.facing_right ? "running_right" : "running_left");
}

Action* Running::input(World& world, GameObject& obj, ActionType action_type) {
    if (action_type == ActionType::None) {
        obj.fsm->transition(Transition::Stop, world, obj);
    }
    else if (action_type == ActionType::Jump) {
        obj.fsm->transition(Transition::Jump, world, obj);
        return new Jump();
    }
    else if (action_type == ActionType::WalkRight) {
        obj.facing_right = true;
        obj.fsm->transition(Transition::WalkRight, world, obj);
        return new WalkRight();
    }
    else if (action_type == ActionType::WalkLeft) {
        obj.facing_right = false;
        obj.fsm->transition(Transition::WalkLeft, world, obj);
        return new WalkLeft();
    }
    else if (action_type == ActionType::RunRight) {
        obj.facing_right = true;
        obj.fsm->transition(Transition::Move, world, obj);
        return new RunRight();
    }
    else if (action_type == ActionType::RunLeft) {
        obj.facing_right = false;
        obj.fsm->transition(Transition::Move, world, obj);
        return new RunLeft();
    }
    else if (action_type == ActionType::AttackRunning) {
        obj.fsm->transition(Transition::AttackingRunning, world, obj);
    }
    return nullptr;
}

void Attacking::on_enter(World&, GameObject& obj) {
    SDL_Log("Attacking::on_enter");
    obj.color = {255, 155, 100,255};
    elapsed = 0.0;
    obj.set_sprite(obj.facing_right ? "attacking_right" : "attacking_left");
}

void Attacking::update(World& world, GameObject& obj, double dt) {
    elapsed += dt;
    if (elapsed >= cooldown) {
        obj.fsm->transition(Transition::Stop, world, obj);
    }
}

Action* Attacking::input(World& world, GameObject& obj, ActionType action_type) {
    if (action_type == ActionType::Attacking) {
        elapsed = 0.0; // reset timer on new shot
    }
    return nullptr;
}

// AttackingWalking
void AttackingWalking::on_enter(World&, GameObject& obj) {
    obj.set_sprite(obj.facing_right ? "walk_attack_right" : "walk_attack_left");
}


Action* AttackingWalking::input(World& world, GameObject& obj, ActionType action_type) {
    if (action_type == ActionType::None) {
        obj.fsm->transition(Transition::Stop, world, obj);
    }
    else if (action_type == ActionType::WalkRight) {
        obj.facing_right = true;
        obj.fsm->transition(Transition::WalkRight, world, obj);
        return new WalkRight();
    }
    else if (action_type == ActionType::WalkLeft) {
        obj.facing_right = false;
        obj.fsm->transition(Transition::WalkLeft, world, obj);
        return new WalkLeft();
    }
    else if (action_type == ActionType::RunRight) {
        obj.facing_right = true;
        obj.fsm->transition(Transition::Move, world, obj);
        return new RunRight();
    }
    else if (action_type == ActionType::RunLeft) {
        obj.facing_right = false;
        obj.fsm->transition(Transition::Move, world, obj);
        return new RunLeft();
    }
    return nullptr;
}

// AttackingRunning
void AttackingRunning::on_enter(World&, GameObject& obj) {
    
    obj.set_sprite(obj.facing_right ? "run_attack_right" : "run_attack_left");
}

Action* AttackingRunning::input(World& world, GameObject& obj, ActionType action_type) {
    if (action_type == ActionType::None) {
        obj.fsm->transition(Transition::Stop, world, obj);
    }
    else if (action_type == ActionType::WalkRight) {
        obj.facing_right = true;
        obj.fsm->transition(Transition::WalkRight, world, obj);
        return new WalkRight();
    }
    else if (action_type == ActionType::WalkLeft) {
        obj.facing_right = false;
        obj.fsm->transition(Transition::WalkLeft, world, obj);
        return new WalkLeft();
    }
    else if (action_type == ActionType::RunRight) {
        obj.facing_right = true;
        obj.fsm->transition(Transition::Move, world, obj);
        return new RunRight();
    }
    else if (action_type == ActionType::RunLeft) {
        obj.facing_right = false;
        obj.fsm->transition(Transition::Move, world, obj);
        return new RunLeft();
    }
    return nullptr;
}

// Patrolling
void Patrolling::on_enter(World& world, GameObject& obj) {
    // set cooldown to a random amount of time 3-10 seconds
    elapsed = 0;
    cooldown = randint(3, 10);
    obj.set_sprite("walking");
    obj.sprites["walking"].flip(!obj.facing_right);
}

Action* Patrolling::input(World& world, GameObject& obj, ActionType action_type) {
    if (elapsed >= cooldown) {
        auto* ai = dynamic_cast<AiInput*>(obj.input);
        if (ai) {
            ai->is_standing = true;
            ai->stand_timer = 0.0;
            ai->stand_cooldown = randint(2, 5);
        }
        obj.fsm->transition(Transition::Stop, world, obj);
        return nullptr;
    }

    if (action_type == ActionType::WalkRight) {
        obj.facing_right = true;
        return new WalkRight();
    }
    else if (action_type == ActionType::WalkLeft) {
        obj.facing_right = false;
        return new WalkLeft();
    }

    return nullptr;
}

void Patrolling::update(World&, GameObject& obj, double dt) {
    elapsed += dt;
    obj.sprites["walking"].flip(!obj.facing_right);
}

// Attack all
void AttackAllEnemies::on_enter(World & world, GameObject & obj) {
    obj.color = {255, 100, 0, 255};
    for (auto& enemy : world.game_objects) {
        enemy->take_damage(obj.damage);
    }
    elapsed = 0;
}

void AttackAllEnemies::update(World & world, GameObject & obj, double dt) {
    elapsed += dt;
    if (elapsed >= cooldown) {
        obj.fsm->transition(Transition::Stop, world, obj);
    }
}

