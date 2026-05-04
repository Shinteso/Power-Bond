//
// Created by Loonj on 2/4/2026.
//
#include "camera.h"

#include "graphics.h"
#include "physics.h"
#include "game_object.h"
#include "projectile.h"

Camera::Camera(Graphics &graphics, float tilesize)
    : graphics{graphics}, tilesize{tilesize}, toggle_grid{false, 0.25}{
    calculate_visible_tiles();
    physics.damping = 0.9;
}

void Camera::calculate_visible_tiles() {
    Vec<int> num_tiles = Vec{graphics.width, graphics.height} / (2 * static_cast<int>(tilesize)) + Vec{1, 1};
    Vec<int> center{static_cast<int>(physics.position.x), static_cast<int>(physics.position.y)};
    visible_max = center + num_tiles;
    visible_min = center - num_tiles;
}

void Camera::set_location(const Vec<float>& new_location) {
    physics.position = new_location;
    calculate_visible_tiles();
}

void Camera::update(const Vec<float>& new_location, float dt) {
    // get the new goal and calculate acceleration then update accordingly
    goal = new_location;
    physics.acceleration = (goal - physics.position) * 4.0f;

    if (distance(physics.position, goal) < 0.5) {
        physics.velocity = {0, 0};
        physics.acceleration = {0, 0};
    }
    else {
        physics.velocity += physics.acceleration * dt;
        physics.velocity *= {physics.damping, physics.damping};
        physics.position += physics.velocity * dt;
    }
    calculate_visible_tiles();
}

Vec<float> Camera::world_to_screen(const Vec<float>& world_position) const {
    // World coordinates (pos y is up) -> screen coordinates (pos y is down)
    Vec<float> pixel = (world_position - physics.position) * static_cast<float>(tilesize);

    // Shift to center
    pixel += Vec<float>{graphics.width / 2.0f, graphics.height / 2.0f};

    // Flip y
    pixel.y = graphics.height - pixel.y;

    return pixel;
}


void Camera::render(const Vec<float> &position, const Color &color,  Vec<int> size, bool filled) const {
    Vec<float> pixel = world_to_screen(position);

    // move from center to bottom left
    pixel.x -= tilesize / 2.0f;
    pixel.y += tilesize / 2.0f;

    // offset by full height so feet stay at bottom anchor
    float screen_w = tilesize * size.x;
    float screen_h = tilesize * size.y;


    SDL_FRect rect{pixel.x, pixel.y - screen_h, screen_w, screen_h};
    graphics.draw(rect, color, filled);
}

void Camera::render(const Tilemap& tilemap) const {
    // screen to world conversion
    // calculate min and max world coordinates and only draw those
    int xmin = std::max(0, visible_min.x);
    int ymin = std::max(0, visible_min.y);
    int xmax = std::min(visible_max.x, tilemap.width - 1);
    int ymax = std::min(visible_max.y, tilemap.height - 1);

    //draw tiles
    for (int y = ymin; y <= ymax; ++y) {
        for (int x = xmin; x <= xmax; ++x) {
            const Tile& tile = tilemap(x, y);

            Vec<float> position{static_cast<float>(x), static_cast<float>(y)};

            Sprite scaled = tile.sprite;
            scaled.scale = tile.sprite.scale * (tilesize / 64.0f);

            render(position, scaled, Vec<int>(1, 1));

            if (toggle_grid.on) {
                render(position, Color{0, 0, 0, 255}, Vec<int>(1,1), false);
            }
        }
    }
}

void Camera::render(const Vec<float> &position, const Sprite &sprite, Vec<int> size, bool flash) const {
    Vec<float> pixel = world_to_screen(position);
    float x_offset = (size.x - 1) * (tilesize / 2.0f);
    pixel.x += x_offset;
    pixel.y += tilesize / 2.0f;
    graphics.draw_sprite(pixel, sprite, flash);
}

void Camera::render_object(const Vec<float>& position, const Sprite& sprite, float y_offset) const {
    Vec<float> pixel = world_to_screen(position);
    pixel.y += y_offset;
    graphics.draw_sprite(pixel, sprite, false);
}

void Camera::render(const GameObject& obj) const {
    // Shift sprite down by certain amount
    Vec<float> position = {
        obj.physics.position.x + obj.physics.shift.x,
        obj.physics.position.y + static_cast<float>(obj.size.y) + obj.physics.shift.y
    };

    Sprite scaled = obj.sprite;
    scaled.scale = obj.sprite.scale * (tilesize / 64.0f);

    if (toggle_grid.on) {
        render(position, obj.color, obj.size);
    }
    render(position, scaled, obj.size, obj.flash_sprite());

    // health bar
    if (obj.health < obj.max_health && obj.max_health > 0 && !dynamic_cast<const Projectile*>(&obj)) {
        float bar_width = tilesize * obj.size.x;
        float bar_height = 4.0f;
        float health_pct = static_cast<float>(obj.health) / static_cast<float>(obj.max_health);

        Vec<float> bar_pos = {
            obj.physics.position.x + obj.physics.shift.x,
            obj.physics.position.y + static_cast<float>(obj.size.y) + obj.physics.shift.y + 0.3f
        };

        Vec<float> bar_pixel = world_to_screen(bar_pos);
        bar_pixel.x -= bar_width / 2.0f;
        bar_pixel.y -= bar_height;

        // background
        SDL_FRect bg_rect{bar_pixel.x, bar_pixel.y, bar_width, bar_height};
        graphics.draw(bg_rect, {255, 0, 0, 200}, true);

        // foreground
        SDL_FRect fg_rect{bar_pixel.x, bar_pixel.y, bar_width * health_pct, bar_height};
        graphics.draw(fg_rect, {0, 255, 0, 200}, true);
    }

}

void Camera::render(const std::vector<Background>& backgrounds) const {
    for (auto background : backgrounds) {
        float shift = physics.position.x / background.distance;
        graphics.draw_sprite({-shift, 0}, background.sprite, false);
    }
}


void Camera::handle_input() {
    const bool* key_states = SDL_GetKeyboardState(NULL);

    if (key_states[SDL_SCANCODE_G]) {
        toggle_grid.flip();
    }
}

void Camera::render_game_over() {
    SDL_FRect full_screen{0.0f, 0.0f, static_cast<float>(graphics.width), static_cast<float>(graphics.height)};
    graphics.draw(full_screen, Color{0,0,0,180}, true);
}

void Camera::render_win() const {
    SDL_FRect full_screen{0.0f, 0.0f, static_cast<float>(graphics.width), static_cast<float>(graphics.height)};
    graphics.draw(full_screen, Color{0,0,0,255}, true);
}

void Camera::set_tilesize(float new_tilesize) {
    tilesize = new_tilesize;
    calculate_visible_tiles();
}