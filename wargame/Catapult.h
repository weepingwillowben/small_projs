#pragma once
#include "Troop.h"
#include "Graphics.h"
class Catapult:
    public Troop
{
public:
    Catapult(void);
    Catapult(Point InSpot);
    ~Catapult(void) = default;
    void Draw();
    void ResetStats();
    bool DestroyedAfterAttack(Troop * AttackTroop);
    bool AttemptAttack(Troop * AttackTroop);
};
