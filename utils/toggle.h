#pragma once
#include "SDL3/SDL_stdinc.h"

class Toggle {
public:
    Toggle(bool on=false, double required_time=0.25);
    void flip();

    bool on;

private:
    double required_time;
    double elapsed;
    Uint64 performance_frequency;
    Uint64 prev_counter;
};
