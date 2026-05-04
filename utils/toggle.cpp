//
// Created by Loonj on 2/9/2026.
//
#include "toggle.h"
#include <SDL3/SDL.h>

#include "../cmake-build-debug/_deps/sdl-src/include/SDL3/SDL_timer.h"

Toggle::Toggle(bool on, double required_time)
    : on{on}, required_time{required_time}, performance_frequency{SDL_GetPerformanceFrequency()}, prev_counter{SDL_GetPerformanceCounter()}{}

void Toggle::flip() {
    Uint64 now = SDL_GetPerformanceCounter();
    elapsed += (now - prev_counter) / (static_cast<double>(performance_frequency));
    prev_counter = now;
    if (elapsed >= required_time) {
        on = !on;
        elapsed = 0;
    }
}
