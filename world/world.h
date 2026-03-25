#pragma once

#include <memory>
#include "tilemap.h"
#include "vec.h"

class GameObject;
class Level;
class Audio;
class World {
public:
    World(const Level& level, Audio& audio);

    void add_platform(float x, float y, float width, float height);
    bool collides(const Vec<float>& position) const;
    GameObject* create_player(const Level& level);
    void load_level(const Level& level);
    void move_to(Vec<float>& position, const Vec<int>& size, Vec<float>& velocity);
    void update(float dt);

    Tilemap tilemap;

private:
    GameObject* player;
    Audio* audio;
};