#include "stdafx.h"
#include "SpecialBuilds.h"

WallClass::WallClass(Point InSpot):WallClass(){
    Spot = InSpot;
};
WallClass::WallClass(void){
    ThisType = TInfo::WallType;
    ResetStats();
    //deals with graphics related fuctions
    Vertical = false;
};
void WallClass::Save(ofstream & File){
    Troop::Save(File);
    File.write((char *)(&Vertical),sizeof(Vertical));
}
void WallClass::Load(ifstream & File){
    Troop::Load(File);
    File.read((char *)(&Vertical),sizeof(Vertical));
}
bool WallClass::DestroyedAfterAttack(Troop * AttackTroop){
    Hitpoints -= AttackTroop->AttackValue;
    if (Hitpoints <= 0)
        return true;
    else
        return false;
}
bool WallClass::AttemptAttack(Troop * AttackTroop){
    if (AttackTroop->TAttackType != TroopAndBuild)
        return false;
    else
        return true;
}
void WallClass::ResetStats(){
    Hitpoints = 2;
    MovementPoints = 0;
    AttackValue = 0;
    Range = 0;
    ExploRange = 0;
    HaveAttacked = false;
}
void WallClass::Draw(){
    if(Vertical)
        DrawTroopPicture(WallTxt,Spot.X,Spot.Y,false,90.0f);
    else
        DrawTroopPicture(WallTxt,Spot.X,Spot.Y);
}
