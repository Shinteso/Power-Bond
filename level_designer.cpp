#include "level_designer.h"
#include "asset_manager.h"

const int TILESIZE = 64;
const int VISIBLE_MAP_WIDTH = 14;
const int VISIBLE_MAP_HEIGHT = 12;
constexpr int COLUMNS = 5;
constexpr float PADDING = 16.0f;
constexpr float ZOOM_STEP = 0.25f;
constexpr float ZOOM_MIN = 0.25f;
constexpr float ZOOM_MAX = 2.0f;


LevelDesigner::LevelDesigner(const std::string &level_name, int width, int height)
    : graphics{"Level Designer", 1280, 720}, tilemap{width, height}, level{level_name},
    dt{0.1}, performance_frequency{SDL_GetPerformanceFrequency()}, prev_counter{SDL_GetPerformanceCounter()}, lag{0.0},
    display_rect{0.0f, 0.0f, graphics.width*(2.0f/3.0f), static_cast<float>(graphics.height)},
    tiles_rect{graphics.width*(2.0f/3.0f), 0.0f, graphics.width*(1.0f/3.0f), static_cast<float>(graphics.height)}{
    update_tile();

    AssetManager::get_level_details(graphics, level);

    // set up palette ids for tile selection
    for (auto& [id, tile] : level.tile_types) {
        palette_ids.push_back(id);
        tile.sprite.shift = {0,0};
    }

    // update the tilemap
    for (const auto& [pos, tile_id] : level.tile_locations) {
        tilemap(pos.x, pos.y) = level.tile_types[tile_id];
    }
}

void LevelDesigner::update_tile() {
    std::string title = "Level Designer | " + level.name;

    if (selected_tile.x >= 0 && selected_tile.y >= 0) {
        title += " - selected: (" + std::to_string(selected_tile.x) + ", " + std::to_string(selected_tile.y) + ")";
    }

    title += " | zoom: " + std::to_string((int)(zoom * 100)) + "% | +/- to zoom";

    graphics.set_title(title);
}

void LevelDesigner::handle_event(SDL_Event *event) {
    // mouse wheel scrolls the tile panel
    if (event->type == SDL_EVENT_MOUSE_WHEEL) {
        float mouse_x, mouse_y;
        SDL_GetMouseState(&mouse_x, &mouse_y);
        graphics.to_logical_coords(mouse_x, mouse_y);

        if (mouse_x > tiles_rect.x) {
            tile_scroll -= static_cast<int>(event->wheel.y);
            if (tile_scroll < 0) tile_scroll = 0;
            int total_rows = (static_cast<int>(palette_ids.size()) + COLUMNS - 1) / COLUMNS;
            int visible_rows = static_cast<int>(tiles_rect.h / (TILESIZE + PADDING));
            int max_scroll = total_rows - visible_rows;
            if (max_scroll < 0) max_scroll = 0;
            if (tile_scroll > max_scroll) tile_scroll = max_scroll;
            // SDL_Log("tile_scroll is now: %d", tile_scroll);
        }
    }

    // use mouse to select a tile position/place
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && event->button.button == SDL_BUTTON_LEFT) {
        float mouse_x = event->button.x;
        float mouse_y = event->button.y;
        graphics.to_logical_coords(mouse_x, mouse_y);

        if (mouse_x > display_rect.x && mouse_y > display_rect.y && mouse_x < display_rect.x + display_rect.w && mouse_y < display_rect.y + display_rect.h) {
            int tilesize = static_cast<int>(TILESIZE * zoom);
            int visible_h = static_cast<int>(display_rect.h / tilesize);
            int grid_x = static_cast<int>((mouse_x - display_rect.x) / tilesize);
            int grid_y = visible_h - 1 - static_cast<int>((mouse_y - display_rect.y) / tilesize);

            int tilemap_x = x_min + grid_x;
            int tilemap_y = y_min + grid_y;

            if (tilemap_x >= 0 && tilemap_x < tilemap.width && tilemap_y >= 0  && tilemap_y < tilemap.height) {
                if (selected_tile.x == tilemap_x && selected_tile.y == tilemap_y) {
                    selected_tile = {-1, -1}; // deselect
                } else {
                    selected_tile = {tilemap_x, tilemap_y};
                }
            }
            update_tile();
            update_tilemap();
        }
        else if (mouse_x > tiles_rect.x && mouse_y > tiles_rect.y && mouse_x < tiles_rect.x + tiles_rect.w && mouse_y < tiles_rect.y + tiles_rect.h) {
            float x_pos = mouse_x - tiles_rect.x - PADDING;
            float y_pos = mouse_y - tiles_rect.y - PADDING;

            if (x_pos >= 0 && y_pos >= 0) {
                int col = static_cast<int>(x_pos / (TILESIZE + PADDING));
                int row = static_cast<int>((y_pos + tile_scroll * (TILESIZE + PADDING)) / (TILESIZE + PADDING));
                int index = row * COLUMNS + col;
                // if clicked again deselect
                if (index >= 0 && index < static_cast<int>(palette_ids.size())) {
                    std::string clicked_id = palette_ids.at(index);
                    selected_palette_id = (clicked_id == selected_palette_id) ? "" : clicked_id;
                }
                update_tilemap();
            }
        }
    }

    // right click to pan
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && event->button.button == SDL_BUTTON_RIGHT) {
        float mouse_x = event->button.x;
        float mouse_y = event->button.y;
        graphics.to_logical_coords(mouse_x, mouse_y);
        if (mouse_x < display_rect.x + display_rect.w) {
            pan_start_x = mouse_x;
            pan_start_y = mouse_y;
            pan_start_x_min = x_min;
            pan_start_y_min = y_min;
        }
    }

    // Watch mouse motion for drag to draw, drag to pan, and hover name
    if (event->type == SDL_EVENT_MOUSE_MOTION) {
        float mouse_x = event->motion.x;
        float mouse_y = event->motion.y;
        graphics.to_logical_coords(mouse_x, mouse_y);

        // Drag to draw using current tile selected
        if (event->motion.state & SDL_BUTTON_LMASK) {
            if (mouse_x > display_rect.x && mouse_y > display_rect.y && mouse_x < display_rect.x + display_rect.w && mouse_y < display_rect.y + display_rect.h) {
                int tilesize = static_cast<int>(TILESIZE * zoom);
                int visible_h = static_cast<int>(display_rect.h / tilesize);
                int grid_x = static_cast<int>((mouse_x - display_rect.x) / tilesize);
                int grid_y = visible_h - 1 - static_cast<int>((mouse_y - display_rect.y) / tilesize);
                int tilemap_x = x_min + grid_x;
                int tilemap_y = y_min + grid_y;
                if (tilemap_x >= 0 && tilemap_x < tilemap.width && tilemap_y >= 0 && tilemap_y < tilemap.height) {
                    selected_tile = {tilemap_x, tilemap_y};
                    update_tilemap();
                    update_tile();
                }
            }
        }

        if (event->motion.state & SDL_BUTTON_MMASK) {
            if (mouse_x > display_rect.x && mouse_y > display_rect.y & mouse_x < display_rect.x + display_rect.w && mouse_y < display_rect.y + display_rect.h) {
                int tilesize = static_cast<int>(TILESIZE * zoom);
                int visible_h = static_cast<int>(display_rect.h / tilesize);
                int grid_x = static_cast<int>((mouse_x - display_rect.x) / tilesize);
                int grid_y = visible_h - 1 - static_cast<int>((mouse_y - display_rect.y) / tilesize);
                int tilemap_x = x_min + grid_x;
                int tilemap_y = y_min + grid_y;
                if (tilemap_x >= 0 && tilemap_x < tilemap.width && tilemap_y >= 0 && tilemap_y < tilemap.height) {
                    tilemap(tilemap_x, tilemap_y) = Tile{};
                }
            }
        }

        // Pan as far as you move the mouse on right click and hold
        if (event->motion.state & SDL_BUTTON_RMASK && mouse_x < display_rect.x + display_rect.w) {
            int tilesize = static_cast<int>(TILESIZE * zoom);
            int visible_w = static_cast<int>(display_rect.w / tilesize);
            int visible_h = static_cast<int>(display_rect.h / tilesize);

            int dx = static_cast<int>((mouse_x - pan_start_x) / tilesize);
            int dy = static_cast<int>((pan_start_y - mouse_y) / tilesize);

            x_min = pan_start_x_min - dx;
            y_min = pan_start_y_min - dy;

            int max_x = tilemap.width - visible_w;
            int max_y = tilemap.height - visible_h;

            if (max_x < 0) max_x = 0;
            if (max_y < 0) max_y = 0;

            if (x_min < 0) x_min = 0;
            if (y_min < 0) y_min = 0;

            if (x_min > max_x) x_min = max_x;
            if (y_min > max_y) y_min = max_y;
        }
        // hover tile name in title bar
        if (mouse_x > tiles_rect.x && mouse_y > tiles_rect.y && mouse_x < tiles_rect.x + tiles_rect.w && mouse_y < tiles_rect.y + tiles_rect.h) {
            float x_pos = mouse_x - tiles_rect.x - PADDING;
            float y_pos = mouse_y - tiles_rect.y - PADDING;
            if (x_pos >= 0 && y_pos >= 0) {
                int col = static_cast<int>(x_pos / (TILESIZE + PADDING));
                int row = static_cast<int>(y_pos / (TILESIZE + PADDING));
                int index = row * COLUMNS + col;
                if (index >= 0 && index < static_cast<int>(palette_ids.size())) {
                    graphics.set_title("Tile: " + palette_ids.at(index));
                }
                else {
                    update_tile();
                }
            }
        }
        else {
            update_tile();
        }
    }
}

void LevelDesigner::input() {
    Uint64 now = SDL_GetPerformanceCounter();
    lag += ((now - prev_counter) / (double)performance_frequency);
    prev_counter = now;
    const bool* keys = SDL_GetKeyboardState(nullptr);

    if ((keys[SDL_SCANCODE_DELETE] || keys[SDL_SCANCODE_BACKSPACE]) && selected_tile.x >= 0 && selected_tile.y >= 0) {
        tilemap(selected_tile.x, selected_tile.y) = Tile{};

        // remove enemy if one exists at this tile
        auto itr = level.enemy_locations.find({static_cast<float>(selected_tile.x), static_cast<float>(selected_tile.y)});
        if (itr != level.enemy_locations.end()) {
            level.enemy_locations.erase(itr);
        }
    }

    if (keys[SDL_SCANCODE_C]) {
        for (int y = 0; y < tilemap.height; ++y) {
            for (int x = 0; x < tilemap.width; ++x) {
                tilemap(x, y) = Tile{};
            }
        }
    }
    if (keys[SDL_SCANCODE_S] && (keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL] || keys[SDL_SCANCODE_LGUI] || keys[SDL_SCANCODE_RGUI])) {
        save();
    }
    if (keys[SDL_SCANCODE_P]) {
        place_player();
    }

    if (lag < dt) {
        return;
    }
    lag = 0;

    if (keys[SDL_SCANCODE_1]) {
        place_enemy("reaper");
    }
    if (keys[SDL_SCANCODE_2]) {
        place_enemy("nightborne");
    }

    // zoom with + and -
    if (keys[SDL_SCANCODE_EQUALS] || keys[SDL_SCANCODE_KP_PLUS]) {
        zoom += ZOOM_STEP;
        if (zoom > ZOOM_MAX) zoom = ZOOM_MAX;
        update_tile();
    }
    else if (keys[SDL_SCANCODE_MINUS] || keys[SDL_SCANCODE_KP_MINUS]) {
        zoom -= ZOOM_STEP;
        if (zoom < ZOOM_MIN) zoom = ZOOM_MIN;
        update_tile();
    }



    if (keys[SDL_SCANCODE_LEFT]) {
        x_min--;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        x_min++;
    }
    else if (keys[SDL_SCANCODE_UP]) {
        y_min++;
    }
    else if (keys[SDL_SCANCODE_DOWN]) {
        y_min--;
    }

    // Switch Levels
    // Next Level
    if (keys[SDL_SCANCODE_N]) {
        current_level++;
        load_level("level_" + std::to_string(current_level));
    }
    // Previous Level
    else if (keys[SDL_SCANCODE_B]) {
        if (current_level > 1) current_level--;
        load_level("level_" + std::to_string(current_level));
    }
}

void LevelDesigner::render() {
    graphics.clear();

    graphics.draw(display_rect, Color{30, 30, 30, 255});
    int tilesize = TILESIZE * zoom;
    int visible_w = static_cast<int>(display_rect.w / tilesize);
    int visible_h = static_cast<int>(display_rect.h / tilesize);

    // clamp y_min to valid range
    int max_y = tilemap.height - visible_h;
    if (max_y < 0) max_y = 0;
    if (y_min > max_y) y_min = max_y;


    for (int y = 0; y < visible_h; ++y) {
        for (int x = 0; x < visible_w; ++x) {
            int tilemap_x = x_min + x;
            int tilemap_y = y_min + y;
            // grid
            if (tilemap_x >= 0 && tilemap_x < tilemap.width && tilemap_y >= 0  && tilemap_y < tilemap.height) {
                float screen_x = display_rect.x + x * tilesize;
                float screen_y = display_rect.y + (visible_h - 1 - y) * tilesize;

                auto& sprite = tilemap(tilemap_x, tilemap_y).sprite;
                Sprite zoomed = sprite;
                zoomed.scale = sprite.scale * zoom;
                zoomed.shift = {sprite.shift.x * zoom, sprite.shift.y * zoom};
                zoomed.center = {sprite.center.x * zoom, sprite.center.y * zoom};

                graphics.draw_sprite({screen_x, screen_y}, zoomed, false);
                SDL_FRect rect{screen_x, screen_y, static_cast<float>(tilesize), static_cast<float>(tilesize)};

                // highlight event tiles
                if (!tilemap(tilemap_x, tilemap_y).event_name.empty()) {
                    graphics.draw(rect, {255, 0, 0, 100});
                }

                Color color = selected_tile == Vec<int>{tilemap_x, tilemap_y} ? Color{255, 255, 0, 255} : Color{0, 0, 0, 255};
                graphics.draw(rect, color, false);

                // render player location as purple
                if (level.player_spawn_location.x == tilemap_x && level.player_spawn_location.y == tilemap_y) {
                    graphics.draw(rect, {255, 0, 255, 100}, true);
                }

                // draw transparent yellow if there is an enemy
                if (level.enemy_locations.contains({static_cast<float>(tilemap_x), static_cast<float>(tilemap_y)})) {
                    graphics.draw(rect, {255, 222, 33, 100}, true);
                }
            }
        }
    }

    graphics.draw(tiles_rect, Color{50, 50, 60, 255});
    draw_tile_display();

    graphics.update();
}

void LevelDesigner::draw_tile_display() {
    for (int i = 0; i < static_cast<int>(palette_ids.size()); ++i) {
        int col = i % COLUMNS;
        int row = i / COLUMNS;

        float x = tiles_rect.x + PADDING + col * (TILESIZE + PADDING);
        float y = tiles_rect.y + PADDING + (row - tile_scroll) * (TILESIZE + PADDING);

        // skip tiles outside the panel
        if (y + TILESIZE <= tiles_rect.y || y >= tiles_rect.y + tiles_rect.h) continue;

        const std::string id = palette_ids.at(i);
        Tile& tile = level.tile_types.at(id);
        graphics.draw_sprite({x, y}, tile.sprite, false);

        if (selected_palette_id == palette_ids.at(i)) {
            SDL_FRect spot{x, y, TILESIZE, TILESIZE};
            graphics.draw(spot, Color{255, 255, 0, 255}, false);
        }
    }
}

void LevelDesigner::update_tilemap() {
    if (selected_tile.x >= 0 && selected_tile.y >= 0 && !selected_palette_id.empty()) {
        tilemap(selected_tile.x, selected_tile.y) = level.tile_types[selected_palette_id];
    }
}

void LevelDesigner::save() {
    level.tile_locations.clear();
    for (int y = 0; y < tilemap.height; ++y) {
        for (int x = 0; x < tilemap.width; ++x) {
            if (tilemap(x, y).sprite.texture_id > -1) {
                level.tile_locations[{x,y}] = tilemap(x,y).id;
            }
        }
    }
    AssetManager::update_level_details(level);
}

void LevelDesigner::place_player() {
    level.player_spawn_location = selected_tile;
}

void LevelDesigner::place_enemy(std::string enemy_name) {
    level.enemy_locations[{static_cast<float>(selected_tile.x), static_cast<float>(selected_tile.y)}] = enemy_name;
}

void LevelDesigner::load_level(const std::string& level_name) {
    try {
        save();

        // preserve from current level
        std::vector<std::string> tile_filenames = level.tile_filenames;
        int width = tilemap.width;
        int height = tilemap.height;

        level = Level{level_name};
        level.tile_filenames = tile_filenames;
        level.width = width;
        level.height = height;

        for (int y = 0; y < tilemap.height; ++y) {
            for (int x = 0; x < tilemap.width; ++x) {
                tilemap(x, y) = Tile{};
            }
        }
        palette_ids.clear();
        selected_palette_id = "";
        selected_tile = {-1, -1};
        x_min = 0;
        y_min = 999;

        try {
            AssetManager::get_level_details(graphics, level);
        } catch (const std::exception& e) {
            SDL_Log("Level not found, starting blank: %s", e.what());
            level.width = tilemap.width;
            level.height = tilemap.height;
            AssetManager::update_level_details(level);
            AssetManager::get_level_details(graphics, level);
        }

        for (auto& [id, tile] : level.tile_types) {
            palette_ids.push_back(id);
            tile.sprite.shift = {0, 0};
        }
        for (const auto& [pos, tile_id] : level.tile_locations) {
            tilemap(pos.x, pos.y) = level.tile_types[tile_id];
        }
        update_tile();
    } catch (const std::exception& e) {
        SDL_Log("load_level failed: %s", e.what());
    }
}
