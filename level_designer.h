#pragma once

#include "graphics.h"
#include "camera.h"
#include "level.h"
#include "vec.h"

class LevelDesigner {
public:
    LevelDesigner(const std::string& level_name, int width, int height);
    void handle_event(SDL_Event* event);
    void input();
    void render();
    void save();

private:
    Graphics graphics;
    Tilemap tilemap;
    Level level;

    Vec<int> selected_tile{-1, -1};
    int y_min{0};
    int x_min{0};
    std::vector<std::string> palette_ids;
    std::string selected_palette_id;

    // level switching
    int current_level{1};
    int current_level_index{0};
    void load_level(const std::string& level_name);

    // timing for scrolling
    double dt;
    Uint64 performance_frequency;
    Uint64 prev_counter;
    double lag;

    // tile panel scroll
    int tile_scroll{0};

    // zoom for display
    float zoom{1.0f};

    // Variable for Panning
    float pan_start_x{0.0f};
    float pan_start_y{0.0f};
    int pan_start_x_min{0};
    int pan_start_y_min{0};

    // UI pieces
    SDL_FRect display_rect;
    SDL_FRect tiles_rect;

    // helper functions
    void update_tile();
    void draw_tile_display();
    void update_tilemap();
    void place_player();
    void place_enemy(std::string enemy_name);
};
