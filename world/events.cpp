//
// Created by Loonj on 3/25/2026.
//

#include "events.h"

#include <iostream>

#include "world.h"
#include "game_object.h"

void NextLevel::perform(World &world, GameObject &obj) {
    world.end_level = true;
}