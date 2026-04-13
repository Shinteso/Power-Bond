//
// Created by Loonj on 3/25/2026.
//
#pragma once
#include <map>
#include <string>

class World;
class GameObject;
class Event;

using Events = std::map<std::string, Event*>;

class Event {
public:
    virtual ~Event() = default;
    virtual void perform(World& world, GameObject& obj) = 0;
};

class NextLevel : public Event {
public:
    void perform(World &world, GameObject &obj) override;
};

