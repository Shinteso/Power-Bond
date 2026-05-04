#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "game.h"

SDL_AppResult SDL_AppInit(void **appstate, int, char*[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    auto game = new Game("Cool Game", 1280, 720);
    *appstate = game;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    auto game = static_cast<Game*>(appstate);
    game->handle_event(event);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    auto game = static_cast<Game*>(appstate);

    game->input();
    game->update();
    game->render();

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult) {
    auto game = static_cast<Game*>(appstate);
    delete game;
}