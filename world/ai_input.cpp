//
// Created by Loonj on 4/24/2026.
//
#include "ai_input.h"

#include "fsm.h"
#include "game_object.h"
#include "world.h"
#include "random.h"

void AiInput::get_input() {

}

void AiInput::handle_input(World &world, GameObject &obj) {
    constexpr float epsilon = 1e-4;

    // If standing wait rand time then move
    if (is_standing) {
        stand_timer += 1.0/60.0;
        if (stand_timer >= stand_cooldown) {
            is_standing = false;
            if (randint(0, 1) == 0) {
                next_action_type = ActionType::WalkRight;
                obj.facing_right = true;
            } else {
                next_action_type = ActionType::WalkLeft;
                obj.facing_right = false;
            }
        }
        obj.fsm->current_state->input(world, obj, ActionType::None);
        return;
    }

    // check for colliding with left wall
    if (world.collides({obj.physics.position.x - epsilon, obj.physics.position.y + obj.size.y - epsilon})) {
        next_action_type = ActionType::WalkRight;
        obj.facing_right = true;
    }



    // check for colliding with right wall
    if (world.collides({obj.physics.position.x + obj.size.x + epsilon, obj.physics.position.y + obj.size.y - epsilon})) {
        next_action_type = ActionType::WalkLeft;
        obj.facing_right = false;
    }

    // check for edge ahead
    if (next_action_type == ActionType::WalkRight) {
        bool wall_ahead = world.collides({obj.physics.position.x + obj.size.x + epsilon, obj.physics.position.y + obj.size.y - epsilon});
        bool ground_ahead = world.collides({obj.physics.position.x + obj.size.x + epsilon, obj.physics.position.y - epsilon});
        if (!wall_ahead && !ground_ahead) {
            // at an edge random choice of fall or turn
            if (randint(0, 10) > 7) {
                next_action_type = ActionType::WalkLeft; // turn around
            }
            // else keep going and fall off
        }
    }
    else if (next_action_type == ActionType::WalkLeft) {
        bool wall_ahead = world.collides({obj.physics.position.x - epsilon, obj.physics.position.y + obj.size.y - epsilon});
        bool ground_ahead = world.collides({obj.physics.position.x - epsilon, obj.physics.position.y - epsilon});
        if (!wall_ahead && !ground_ahead) {
            if (randint(0, 10) > 7) {
                next_action_type = ActionType::WalkRight;
            }
        }
    }

    Action* action  = obj.fsm->current_state->input(world, obj, next_action_type);

    if (action) {
        action->perform(world, obj);
        delete action;
    }
}
