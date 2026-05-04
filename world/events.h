//
// Created by Loonj on 3/25/2026.
//
#pragma once
#include <map>
#include <string>

class World;
class GameObject;

class Event {
public:
    virtual ~Event() = default;
    virtual void perform(World& world, GameObject& obj) = 0;
};

using Events = std::map<std::string, Event*>;

class NextLevel : public Event {
public:
    void perform(World& world, GameObject& obj) override;
};

