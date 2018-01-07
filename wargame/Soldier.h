#pragma once
#include "Troop.h"
#include "Graphics.h"
class Soldier:
    public Troop
{
public:
    Soldier(void);
    Soldier(Point InSpot);
    ~Soldier(void) = default;
    void Draw();
    bool DestroyedAfterAttack(Troop * AttackTroop);
    bool AttemptAttack(Troop * AttackTroop);
    void ResetStats();
};

