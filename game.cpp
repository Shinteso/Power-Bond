#include "game.h"

#include <iostream>

#include "ai_input.h"
#include "input.h"
#include "asset_manager.h"
#include "fsm.h"
#include "keyboard_input.h"
#include "states.h"
#include "level.h"

Game::Game(std::string title, int width, int height)
    : graphics{title, width, height}, camera{graphics, 64}, dt{1.0/60.0}, performance_frequency{SDL_GetPerformanceFrequency()}, prev_counter{SDL_GetPerformanceCounter()}, lag{0.0} {

    // load events
    get_events();

    // Give player its assents and set correct state
    create_player();
    AssetManager::get_game_object_details("player", graphics, *player);

    // load the first "level"
    load_level();
}

Game::~Game() {
    delete world;
    for (auto [_, event]: events) {
        delete event;
    }
}

void Game::handle_event(SDL_Event* event) {
    switch (mode) {
    case GameMode::Playing:
        auto action = player -> input-> collect_discrete_event(event);
        if (action) {
            action->perform(*world, *player);
            delete action;
        }
        break;
    }
}

void Game::input() {
    switch (mode) {
    case GameMode::Playing:
        player->input->get_input();
        camera.handle_input();
        break;
    }
}

void Game::update() {
    Uint64 now = SDL_GetPerformanceCounter();
    lag += (now - prev_counter) / (float)performance_frequency;
    prev_counter = now;
    while (lag >= dt) {
        switch (mode) {
        case GameMode::Playing:
            for (auto obj : world->game_objects) {
                obj->input->handle_input(*world, *obj);
            }

            world->update(dt);
            // put the camera slightly ahead of the player
            float L = length(player->physics.velocity);
            Vec displacement = 8.0f * player->physics.velocity / (1.0f + L);
            camera.update(player->physics.position + displacement, dt);

            if (world->end_level) {
                    load_level();
            }
            // check for game over
            if (world->end_game) {
                mode = GameMode::GameOver;
            }
            break;
        }
        lag -= dt;
    }
}


void Game::render() {
    // clear
    graphics.clear();

    //draw backgrounds
    camera.render(world->backgrounds);

    // draw the world
    camera.render(world->tilemap);


    if (mode != GameMode::GameOver && mode != GameMode::Win) {
        // draw the player
        camera.render(*player);

        // draw enemies
        for (auto& obj : world->game_objects) {
            camera.render(*obj);
        }

        // draw projectiles
        for (auto& projectile : world->projectiles) {
            camera.render(*projectile);
        }
    }

    // game end
   if (mode == GameMode::GameOver) {
        camera.render_game_over();
    }

    if (mode == GameMode::Win) {
        camera.render_win();
    }


    // update
    graphics.update();
}

void Game::get_events() {
    events["next_level"] = new NextLevel();
}

void Game::load_level() {
    std::string level_name = "level_" + std::to_string(++current_level);
    Level level{level_name};
    AssetManager::get_level_details(graphics, level);

    // create the world
    delete world;
    world = new World(level, audio, player.get(), events);

    // Get available items
    AssetManager::get_available_items("items", graphics, *world);

    // assets for objs
    for (auto& obj : world->game_objects) {
        if (obj == world->player) continue;
        update_enemy(*obj);
        AssetManager::get_game_object_details(obj->obj_name + "-enemy", graphics, *obj, true);
    }

    player->physics.position = {static_cast<float>(level.player_spawn_location.x), static_cast<float>(level.player_spawn_location.y)};
    player->fsm->current_state->on_enter(*world, *player);

    if (current_level == 3) {
        camera.set_tilesize(21.0f);
        camera.set_location({30.0f, 15.0f});
    } else {
        camera.set_tilesize(64.0f);
        camera.set_location(player->physics.position);
    }

    audio.play_sounds("background", true);
}


void Game::create_player() {
    // Create fsm
    Transitions transitions = {
        {{StateType::Standing, Transition::Jump}, StateType::InAir},
        {{StateType::InAir, Transition::Stop}, StateType::Standing},
        {{StateType::Standing, Transition::WalkRight}, StateType::Walking},
        {{StateType::Standing, Transition::WalkLeft}, StateType::Walking},
        {{StateType::Standing, Transition::Move}, StateType::Running},
        {{StateType::Walking, Transition::Stop}, StateType::Standing},
        {{StateType::Walking, Transition::Jump}, StateType::InAir},
        {{StateType::Walking, Transition::Move}, StateType::Running},
        {{StateType::Walking, Transition::WalkRight}, StateType::Walking},
        {{StateType::Walking, Transition::WalkLeft}, StateType::Walking},
        {{StateType::Running, Transition::Stop}, StateType::Standing},
        {{StateType::Running, Transition::Jump}, StateType::InAir},
        {{StateType::Running, Transition::WalkRight}, StateType::Walking},
        {{StateType::Running, Transition::WalkLeft}, StateType::Walking},
        {{StateType::InAir, Transition::Move}, StateType::Running},
        {{StateType::InAir, Transition::WalkRight}, StateType::Walking},
        {{StateType::InAir, Transition::WalkLeft}, StateType::Walking},
        {{StateType::Standing, Transition::Attacking}, StateType::Attacking},
        {{StateType::Attacking, Transition::Stop}, StateType::Standing},
        {{StateType::Standing, Transition::AttackAll}, StateType::AttackAllEnemies},
        {{StateType::AttackAllEnemies, Transition::Stop}, StateType::Standing},
        {{StateType::Walking, Transition::AttackingWalking}, StateType::WalkAttacking},
        {{StateType::WalkAttacking, Transition::Stop}, StateType::Standing},
        {{StateType::WalkAttacking, Transition::AttackingRunning}, StateType::RunAttacking},
        {{StateType::Running, Transition::AttackingRunning}, StateType::RunAttacking},
        {{StateType::RunAttacking, Transition::Stop}, StateType::Standing},
        {{StateType::RunAttacking, Transition::AttackingWalking}, StateType::WalkAttacking},
    };

    States states = {
        {StateType::Standing, new Standing()},
        {StateType::InAir, new InAir()},
        {StateType::Walking, new Walking()},
        {StateType::Running, new Running()},
        {StateType::Attacking, new Attacking()},
        {StateType::WalkAttacking, new AttackingWalking()},
        {StateType::RunAttacking, new AttackingRunning()},
    };

    FSM* fsm = new FSM{transitions, states, StateType::Standing};

    // player input
    KeyboardInput* input = new KeyboardInput;

    player = std::make_unique<GameObject>("player", fsm, input, Color{0, 0, 0});
}

void Game::update_enemy(GameObject &obj) {
    Transitions transitions;
    States states;

    if (obj.obj_name == "reaper" || obj.obj_name == "nightborne") {
        transitions = {
            {{StateType::Standing, Transition::Move}, StateType::Patrolling},
            {{StateType::Standing, Transition::WalkRight}, StateType::Patrolling},
            {{StateType::Standing, Transition::WalkLeft}, StateType::Patrolling},
            {{StateType::Patrolling, Transition::Stop}, StateType::Standing},
        };
        states = {
            {StateType::Standing, new Standing()},
            {StateType::Patrolling, new Patrolling()}
        };
    }
    else {
        // Throw an error
        throw std::runtime_error("Unknown enemy type: " + obj.obj_name);
    }

    FSM* fsm = new FSM{transitions, states, StateType::Patrolling};
    obj.fsm = fsm;

    Input* input = new AiInput();
    input->next_action_type = ActionType::RunRight;
    obj.input = input;
}
