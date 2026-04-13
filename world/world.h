#pragma once

#include <memory>
#include "tilemap.h"
#include "vec.h"
#include "events.h"

class GameObject;
class Level;
class Audio;
class World {
public:
    World(const Level& level, Audio& audio, GameObject* player, Events events);

    void add_platform(float x, float y, float width, float height);
    bool collides(const Vec<float>& position) const;
    void load_level(const Level& level);
    void move_to(Vec<float>& position, const Vec<int>& size, Vec<float>& velocity);
    void update(float dt);

    Tilemap tilemap;
    bool end_level{false};

private:
    GameObject* player;
    Audio* audio;
    Events events;

    void touch_tiles(GameObject& obj);
};