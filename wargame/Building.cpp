
#include "stdafx.h"
#include "Building.h"
#include "Troop.h"

int GetBuildingCost(BuildingTypes Build){
    switch (Build){
    case SFarm:return 50;
    case LFarm:return 100;
    case CataFact:return 500;
    case TrainCenter:return 100;
    case BA_Factory:return 200;
    case TorchFactory:return 200;
    case Armory:return 300;
    case Stable:return 500;
    }
    return 1000000;//should not happen
}
int GetBuildingSize(BuildingTypes Build){
    switch (Build){
    case SFarm:return 4;
    case LFarm:return 4;
    case CataFact:return 4;
    case TrainCenter:return 4;
    case BA_Factory:return 4;
    case TorchFactory:return 4;
    case Armory:return 4;
    case Stable:return 4;
    }
}
int GetBuildingIncome(BuildingTypes Build){
    if(Build == SFarm)
        return 3;
    else if(Build == LFarm)
        return 5;
    else
        return 0;
}
Pictures TypeToPic(BuildingTypes Build){
    switch (Build){
    case SFarm:return SmallFarmTxt;
    case LFarm:return LargeFarmTxt;
    case CataFact:return CataFactTxt;
    case TrainCenter:return TrainCenterTxt;
    case BA_Factory:return BA_FactTxt;
    case TorchFactory:return TorchFactTxt;
    case Armory:return ArmoryTxt;
    case Stable:return StableTxt;
    case WallSelect:return WallIconTxt;
    default:return SmallFarmTxt;//should not happen
    }
}
int BuyAmmountConverter(BuildingTypes Build){
    switch (Build){
    case SFarm:return 0;
    case LFarm:return 0;
    case CataFact:return 1;
    case TrainCenter:return 5;
    case BA_Factory:return 5;
    case TorchFactory:return 5;
    case Armory:return 5;
    case Stable:return 5;
    }
}
Building::Building(BuildingTypes InBuildType,int x1,int y1,int x2, int y2){
    Type = InBuildType;
    int LowBuildX,HighBuildX;
    int LowBuildY,HighBuildY;
    if (x1 < x2){
        LowBuildX = x1;
        HighBuildX = x2;
    }
    else {
        HighBuildX = x1;
        LowBuildX = x2;
    }
    if (y1 < y2){
        LowBuildY = y1;
        HighBuildY = y2;
    }
    else {
        HighBuildY = y1;
        LowBuildY = y2;
    }
    int BuildSize = GetBuildingSize(Type);

    if(BuildSize == 4){
        Place.resize(4);
        Place[0] = CreatePoint(LowBuildX, LowBuildY);
        Place[1] = CreatePoint(HighBuildX, LowBuildY);
        Place[2] = CreatePoint(LowBuildX, HighBuildY);
        Place[3] = CreatePoint(HighBuildX, HighBuildY);
    }
    DrawCen = Place[0];
    BuyAmmount = 0;//makes it so that the player has to wait a turn to use this building
}
Building::~Building(void){}
void Building::Save(ofstream & File){
    File.write((char *)(&Type),sizeof(Type));
    File.write((char *)(&BuyAmmount),sizeof(BuyAmmount));
    int NumOfPlaces = Place.size();
    File.write((char *)(&NumOfPlaces), sizeof(NumOfPlaces));
    File.write((char *)(&DrawCen), sizeof(DrawCen));
    File.write((char *)(Place.data()), sizeof(Point)*Place.size());
}
void Building::Load(ifstream & File){
    File.read((char *)(&Type),sizeof(Type));
    File.read((char *)(&BuyAmmount),sizeof(BuyAmmount));
    int NumOfPlaces;
    File.read((char *)(&NumOfPlaces), sizeof(NumOfPlaces));;
    File.read((char *)(&DrawCen), sizeof(DrawCen));
    Place.resize(NumOfPlaces);
    File.read((char *)(Place.data()), sizeof(Point)*Place.size());
}
void Building::Draw(){
    for (Point & P:Place){
        DrawBuildPicture(TypeToPic(Type), P.X - DrawCen.X, P.Y - DrawCen.Y, DrawCen.X, DrawCen.Y);
    }
}
void Building::ResetStats(){
    BuyAmmount = BuyAmmountConverter(Type);
}
int Building::GetCoordVar(Point Spot){
    for(int CVar :range(Place.size())){
        if (Place[CVar] == Spot)
            return CVar;
    }
}
int Building::GetNumOfSquares(){
    return Place.size();
}
bool Building::GetOverallExist(){
    return Place.size() == 0;
}
bool Building::DestroyedAfterAttack(Point Loc,Troop * AttackingTroop){
    if(AttackingTroop->TAttackType != TroopAndBuild)
        return false;

    int Coord = GetCoordVar(Loc);
    Place.erase(Place.begin() + Coord);

    return (Place.size() == 0);
}
bool Building::AttemptAttack(Troop * AttackingTroop){
    return (AttackingTroop->TAttackType == TroopAndBuild);
}
void Building::IterOverBuildFromDis(Array2d<int> & PlayerOcc,int DistanceFromBuild, function<void(Point)> IterFunc){
    Point Cen = GetAvPoint();
    //gets the maximum distance from the average point(not necessarily an actual part of the building!)
    int MaxDis = 0;
    for (Point BP : Place)
        MaxDis = max(MaxDis, GetBoardDistance(Cen, BP));

    int TDis = MaxDis + DistanceFromBuild;
    for (Point PosPoint : SquareIterate(Cen,TDis)){
        if (!BlankPoint(PosPoint))
            continue;
        //makes the final check that this square is valid
        for (Point BP : Place){
            if (GetBoardDistance(BP,PosPoint) <= DistanceFromBuild){
                IterFunc(PosPoint);
                continue;
            }
        }
    }
}
Point Building::GetAvPoint(){
    Point CenP = {0,0};
    for(Point & P : Place){
        CenP += P;
    }
    CenP /= Place.size();
    return CenP;
}
int Building::GetIncome(){
    return GetBuildingIncome(Type);
}
