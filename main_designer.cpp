#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "level_designer.h"


SDL_AppResult SDL_AppInit(void **appstate, int, char**) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    auto designer = new LevelDesigner("level_1", 62, 30);
    *appstate = designer;

    return SDL_APP_CONTINUE; // Continue Program

}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event *event) {
    auto designer = static_cast<LevelDesigner*>(appstate);
    if (event->type == SDL_EVENT_QUIT) {
        // Save progress
        designer->save();
        return SDL_APP_SUCCESS;
    }

    // only log non-mouse-motion events
    // if (event->type != SDL_EVENT_MOUSE_MOTION) {
    //     SDL_Log("event type: %u", event->type);
    // }

    designer->handle_event(event);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    auto designer = static_cast<LevelDesigner*>(appstate);

    designer->input();
    designer->render();

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult) {
    auto designer = static_cast<LevelDesigner*>(appstate);
    delete designer;
}
