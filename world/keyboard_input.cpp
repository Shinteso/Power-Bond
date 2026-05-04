//
// Created by Loonj on 2/25/2026.
//
#include "game_object.h"
#include "fsm.h"
#include "keyboard_input.h"

void KeyboardInput::get_input() {
    if (next_action_type == ActionType::Jump) return;

    const bool* key_states = SDL_GetKeyboardState(NULL);
    bool shift = key_states[SDL_SCANCODE_LSHIFT] || key_states[SDL_SCANCODE_RSHIFT];

    if (key_states[SDL_SCANCODE_D] && shift) {
        next_action_type = ActionType::RunRight;
    }
    else if (key_states[SDL_SCANCODE_D]) {
        next_action_type = ActionType::WalkRight;
    }
    else if (key_states[SDL_SCANCODE_A] && shift) {
        next_action_type = ActionType::RunLeft;
    }
    else if (key_states[SDL_SCANCODE_A]) {
        next_action_type = ActionType::WalkLeft;
    }
    if (key_states[SDL_SCANCODE_C]) {
        next_action_type = ActionType::Attacking;
    }

    // hold to shoot with cooldown
    if (key_states[SDL_SCANCODE_F]) {
        if (shoot_cooldown <= 0.0) {
            shoot_pending = true;
            shoot_cooldown = shoot_rate;
            next_action_type = ActionType::Attacking;
        }
    }
    shoot_cooldown -= 1.0/60.0;
    if (shoot_cooldown < 0.0) shoot_cooldown = 0.0;
}

void KeyboardInput::handle_input(World &world, GameObject &obj) {
    if (shoot_pending) {
        shoot_pending = false;
        ShootBullet action;
        action.perform(world, obj);
    }

    Action* action = obj.fsm->current_state->input(world, obj, next_action_type);

    // consume the action
    next_action_type = ActionType::None;
    if (action != nullptr) {
        action->perform(world, obj);
        delete action;
    }
}

Action* KeyboardInput::collect_discrete_event(SDL_Event* event) {
    if (event->type == SDL_EVENT_KEY_DOWN && event->key.repeat == 0) {
        if (event->key.scancode == SDL_SCANCODE_SPACE) {
            next_action_type = ActionType::Jump;
        }

        if (event->key.scancode == SDL_SCANCODE_M) {
            next_action_type = ActionType::AttackAll;
        }
    }
    return nullptr;
}
