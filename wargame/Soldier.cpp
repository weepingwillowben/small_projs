#include "stdafx.h"
#include "Soldier.h"


Soldier::Soldier(Point InSpot):Soldier(){
    Spot = InSpot;
}
Soldier::Soldier(){
    ThisType = TInfo::SoldierType;
    ResetStats();

    //when you create a soldier, it cannot move or attack, but it does defend itself
    MovementPoints = 0;
    HaveAttacked = true;
}
void Soldier::Draw(){
    using namespace Attach;
    DrawTroopPicture(SoldierTxt,Spot.X,Spot.Y);
    if(Attcs.has(Armor))
        DrawTroopPicture(ArmorTxt, Spot.X, Spot.Y);
    if(Attcs.has(BA))
        DrawTroopPicture(BATxt, Spot.X, Spot.Y);
    if(Attcs.has(Horse))
        DrawTroopPicture(HorseTxt, Spot.X, Spot.Y);
    if(Attcs.has(Torch))
        DrawTroopPicture(TorchTxt, Spot.X, Spot.Y);
}
void Soldier::ResetStats(){
    using namespace Attach;
    MovementPoints = 2 + Attcs.has(Horse)*2;
    Hitpoints = 1 + Attcs.has(Armor);
    TAttackType = Attcs.has(Torch) ? TroopAndBuild : TroopOnly;
    AttackValue = 1;
    ExploRange = 1;
    Range = 2 + Attcs.has(BA) * 2;
    HaveAttacked = false;
}
bool Soldier::DestroyedAfterAttack(Troop * AttackTroop){
    Hitpoints -= AttackTroop->AttackValue;
    if(Hitpoints <= 0)
        return true;
    else
        return false;
}
bool Soldier::AttemptAttack(Troop * AttackTroop){
    return true;
}
