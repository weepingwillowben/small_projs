#pragma once
#include "Graphics.h"
#include "Globals.h"
int GetBuildingSize(BuildingTypes Build);
int GetBuildingCost(BuildingTypes Build);
int GetBuildingIncome(BuildingTypes Build);
Pictures TypeToPic(BuildingTypes Build);
int BuyAmmountConverter(BuildingTypes Build);
//this class is built to have a variable and arbitrary grouping of squares
class Building
{
public:
    Building(void){};
    Building(BuildingTypes InBuildType,int x1,int y1,int x2, int y2);
    ~Building(void);
    BuildingTypes Type;
    int	BuyAmmount;
    Point DrawCen;
    vector<Point> Place;
    void Save(ofstream & File);
    void Load(ifstream & File);
    int GetIncome();
    int GetNumOfSquares();
    Point GetAvPoint();
    void IterOverBuildFromDis(Array2d<int> & PlayerOcc, int DistanceFromBuild, function<void(Point)> IterFunc);//IterFunc takes one argument, Point
    int GetSize(){ return Place.size(); }
    int GetCost(){return GetBuildingCost(Type);};
    void Draw();
    bool AttemptAttack(Troop * AttackingTroop);
    bool DestroyedAfterAttack(Point Loc,Troop * AttackingTroop);
    bool GetOverallExist();
    int GetCoordVar(Point Spot);
    void ResetStats();
};
