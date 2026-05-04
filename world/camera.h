//
// Created by Loonj on 2/4/2026.
//
#pragma once

#include "tilemap.h"
#include "vec.h"
#include "toggle.h"
#include "physics.h"


class Graphics;
class Tilemap;
class Color;
class Sprite;
class GameObject;

class Camera {
public:
    Camera(Graphics& graphics, float tilesize);

    void update(const Vec<float>& new_location, float dt);
    Vec<float> world_to_screen(const Vec<float>& world_position) const;
    void set_location(const Vec<float>& new_location);

    void render(const Vec<float>& position, const Color& color, Vec<int> size, bool filled=true) const;
    void render(const Tilemap& tilemap) const;
    void render(const Vec<float>& position, const Sprite& sprite, Vec<int> size, bool flash=false) const;
    void render_object(const Vec<float>& position, const Sprite& sprite, float y_offset) const;
    void render(const GameObject& obj) const;
    void render(const std::vector<Background>& backgrounds) const;

    void handle_input();
    void render_game_over();
    void render_win() const;

    void set_tilesize(float new_tilesize);

private:
    Graphics& graphics;
    float tilesize;


    void calculate_visible_tiles();
    Vec<int> visible_min, visible_max;
    Vec<float> goal;

    Physics physics;

    Toggle toggle_grid;
};
