#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "sprite.h"

class Color {
public:
    int red{255}, green{0}, blue{0}, alpha{255};
};

class Graphics {
public:
    Graphics(std::string title, int width, int height);
    void draw(const SDL_FRect& rect, const Color& color, bool filled=true);
    void clear();
    void update();
    void set_title(const std::string& title);

    int get_texture_id(const std::string& img_filename);
    void draw_sprite(const Vec<float>& pixel, const Sprite& sprite);

    const int width;
    const int height;

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::vector<SDL_Texture*> textures;
    std::unordered_map<std::string, int> texture_ids;
};