//
// Created by Loonj on 2/13/2026.
//
#pragma once

class World;
class GameObject;

enum class ActionType {
    None,
    Jump,
    WalkLeft,
    WalkRight,
    RunLeft,
    RunRight,
    Attacking,
    AttackWalking,
    AttackRunning,
    AttackAll
};

class Action {
public:
    virtual ~Action() = default;
    virtual void perform(World& world, GameObject& obj) = 0;
};

class Jump : public Action {
    void perform(World&, GameObject& obj) override;
};

class WalkRight : public Action {
    void perform(World&, GameObject& obj) override;
};

class WalkLeft : public Action {
    void perform(World&, GameObject& obj) override;
};

class RunLeft : public Action {
    void perform(World&, GameObject& obj) override;
};

class RunRight : public Action {
    void perform(World&, GameObject& obj) override;
};

class AttackAll : public Action {
public:
    void perform(World& world, GameObject& obj) override;
};

class ShootBullet : public Action {
public:
    void perform(World& world, GameObject& obj) override;
};