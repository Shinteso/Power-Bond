#pragma once

#include <map>
#include <memory>

#include "audio.h"
#include "events.h"
#include "quadtree.h"
#include "tilemap.h"
#include "vec.h"
#include "projectile.h"

class GameObject;
class Level;
class Audio;
class Event;


class World {
public:
    World(const Level& level, Audio& audio, GameObject* player, Events events);
    ~World();

    void add_platform(float x, float y, float width, float height);
    bool collides(const Vec<float>& position) const;
    void move_to(Vec<float>& position, const Vec<int>& size, Vec<float>& velocity);
    void update(double dt);
    void update_object(GameObject* obj, double dt);
    void load_level(const Level& level);

    Tilemap tilemap;
    bool end_level{false};

    GameObject* player;
    Audio* audio;
    std::vector<GameObject*> game_objects;
    std::vector<Projectile*> projectiles;
    Events events;

    void touch_tiles(GameObject& obj);

    QuadTree quadtree;
    void build_quadtree();
    bool end_game{false};

    // items
    std::map<std::string, std::function<GameObject*()>> available_items;

    //backgrounds
    std::vector<Background> backgrounds;
};