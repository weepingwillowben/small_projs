#pragma once
#include "Graphics.h"
#include "Building.h"
#include "Troop.h"
class WallClass:
    public Troop
{
public:
    WallClass(void);
    WallClass(Point InSpot);
    ~WallClass(void){};
    //all of these extra variables are graphics only
    bool Vertical;
    void Save(ofstream & File);
    void Load(ifstream & File);
    void Draw();
    bool DestroyedAfterAttack(Troop * AttackTroop);
    bool AttemptAttack(Troop * AttackTroop);
    void ResetStats();
};
