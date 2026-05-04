//
// Created by Loonj on 2/18/2026.
//
#pragma once
#include "fsm.h"
#include "world.h"

class Standing: public State {
public:
    void on_enter(World&, GameObject&) override;
    Action* input(World&, GameObject&, ActionType) override;
};

class InAir: public State {
public:
    void on_enter(World &, GameObject &) override;
    void update(World &, GameObject &, double dt) override;
    Action* input(World&, GameObject&, ActionType) override;

    const double cooldown = .1;
    double elapsed = 0;
};

class Walking : public State {
public:
    void on_enter(World&, GameObject&) override;
    Action* input(World&, GameObject&, ActionType) override;
};

class Running : public State {
public:
    void on_enter(World&, GameObject&) override;
    Action* input(World&, GameObject&, ActionType) override;
};

class Patrolling: public Walking {
public:
    void on_enter(World&, GameObject&) override;
    Action* input(World&, GameObject&, ActionType) override;
    void update(World&, GameObject&, double dt) override;

    double elapsed{0.0 };
    double cooldown{0.0};
};

class AttackAllEnemies : public State {
public:
    virtual void on_enter(World&, GameObject&) override;
    virtual void update(World&, GameObject&, double dt) override;

    double elapsed = 0;
    double cooldown = 2;
};

class Attacking: public State {
public:
    void on_enter(World&, GameObject&) override;
    Action* input(World&, GameObject&, ActionType) override;
    void update(World&, GameObject&, double dt) override;

    double elapsed{0.0};
    double cooldown{0.5};
};

// while walking
class AttackingWalking: public State {
public:
    void on_enter(World&, GameObject&) override;
    Action* input(World&, GameObject&, ActionType) override;
};

// while running
class AttackingRunning: public State {
public:
    void on_enter(World&, GameObject&) override;
    Action* input(World&, GameObject&, ActionType) override;
};

