#include "stdafx.h"
#include "Catapult.h"

Catapult::Catapult(Point InSpot):Catapult(){
    Spot = InSpot;
}
Catapult::Catapult(){
    ThisType = TInfo::CatapultType;

    //when you create a catapult, it cannot move or attack, but it does defend itself
    ResetStats();
    MovementPoints = 0;
    HaveAttacked = true;
}
void Catapult::Draw(){
    bool HasHorse = Attcs.has(Attach::CataHorse);
    if(HasHorse)
        DrawTroopPicture(HorseTxt,Spot.X,Spot.Y,true);
    DrawTroopPicture(CatapultTxt,Spot.X,Spot.Y,false);
    //DrawTroopPicture(CatapultTxt,XSpot,YSpot);
    if(HasHorse)
        DrawTroopPicture(HorseTxt,Spot.X,Spot.Y + 1/6.0f,true);
}
void Catapult::ResetStats(){
    MovementPoints = 2 + Attcs.has(Attach::CataHorse)*2;
    Hitpoints = 1;
    TAttackType = TroopAndBuild;
    AttackValue = 1;
    ExploRange = 1;
    Range = 5;
    HaveAttacked = false;
}
bool Catapult::DestroyedAfterAttack(Troop * AttackTroop){
    Hitpoints -= AttackTroop->AttackValue;
    if(Hitpoints <= 0)
        return true;
    else
        return false;
}
bool Catapult::AttemptAttack(Troop * AttackTroop){
    return true;
}
