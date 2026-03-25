//
// Created by Loonj on 2/4/2026.
//
#include "camera.h"

#include "graphics.h"
#include "physics.h"
#include "game_object.h"

Camera::Camera(Graphics& graphics, float tilesize)
    : graphics{graphics}, tilesize{tilesize}{
    calculate_visible_tiles();
}

void Camera::calculate_visible_tiles() {
    Vec<int> num_tiles = Vec{graphics.width, graphics.height} / (2 * static_cast<int>(tilesize)) + Vec{1, 1};
    Vec<int> center{static_cast<int>(physics.position.x), static_cast<int>(physics.position.y)};
    visible_max = center + num_tiles;
    visible_min = center - num_tiles;
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

void Camera::handle_input() {
    const bool* key_states = SDL_GetKeyboardState(NULL);

    if (key_states[SDL_SCANCODE_G]) {
        grid_toggle.flip();
    }
}

void Camera::update(const Vec<float>& new_location, float dt) {
    goal = new_location;
    physics.acceleration = (goal - physics.position) * 10.0f;

    physics.velocity += 0.5f * physics.acceleration * dt;
    physics.position += physics.velocity * dt;
    physics.velocity += 0.5f * physics.acceleration * dt;
    physics.velocity *= {physics.damping, physics.damping};

    calculate_visible_tiles();
}

void Camera::set_location(const Vec<float>& new_location) {
    physics.position = new_location;
    calculate_visible_tiles();
}


void Camera::render(const Vec<float>& position, const Color& color, bool filled) const {
    Vec<float> pixel = world_to_screen(position);
    pixel -= Vec{tilesize/2, tilesize/2}; // center on tile
    SDL_FRect rect{pixel.x, pixel.y, tilesize, tilesize};
    graphics.draw(rect, color, filled);
}

void Camera::render(const Tilemap& tilemap) const {
    int xmin = std::max(0, visible_min.x);
    int ymin = std::max(0, visible_min.y);
    int xmax = std::min(visible_max.x, tilemap.width - 1);
    int ymax = std::min(visible_max.y, tilemap.height - 1);

    //draw tiles
    for (int y = ymin; y <= ymax; ++y) {
        for (int x = xmin; x <= xmax; ++x) {
            const Tile& tile = tilemap(x, y);
            Vec<float> position{static_cast<float>(x), static_cast<float>(y)};

            // Draw tile sprite of color rectangle
            render(position, tile.sprite);

            if (grid_toggle.on) {
                render (position, {0, 0, 0, 0}, false);
            }
        }
    }
}

void Camera::render(const Vec<float>& position, const Sprite& sprite) const {
    Vec<float> pixel = world_to_screen(position);
    graphics.draw_sprite(pixel, sprite);
}


void Camera::render(const GameObject& obj) const {
    if (grid_toggle.on) {
        render(obj.physics.position, obj.color);
    }
    render(obj.physics.position, obj.sprite);
}


