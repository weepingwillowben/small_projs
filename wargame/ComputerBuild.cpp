#include "stdafx.h"
#include "ComputerPlayer.h"
double TroopBuyRatios(TInfo::TroopTypes TType){
    using namespace TInfo;
    switch (TType){
    case SoldierType:return 4.0;
    case CatapultType:return 1.0;
    case WallType:return 0.00000000000000000000000000000001;
    }
}
int DistanceToVector(Point P,Vector V){
    auto Between = [](int XComp,int XB1,int XB2){
        return ((XB1 <= XComp && XB2 >= XComp) || (XB1 >= XComp && XB2 <= XComp));
    };
    int XDis = abs(V.Dest.X - V.Orig.X);
    int YDis = abs(V.Dest.Y - V.Orig.Y);
    int XVMin = min(V.Dest.X , V.Orig.X);
    int YVMin = min(V.Dest.Y , V.Orig.Y);
    if (Between(P.X, V.Dest.X, V.Orig.X) && Between(P.Y, V.Dest.Y, V.Orig.Y))
        return 0;
    else{
        int MinX = min(abs(P.X - V.Dest.X), abs(P.X - V.Orig.X));
        int MinY = min(abs(P.Y-V.Dest.Y),abs(P.Y-V.Orig.Y));
        return min(MinX,MinY);
    }
}
bool BuildIsMilitary(BuildingTypes Build){
    return Build == CataFact || Build == TrainCenter || Build == TorchFactory || Build == BA_Factory || Build == Stable || Build == Armory;
}
bool BuildIsCivilian(BuildingTypes Build){
    return Build == SFarm || Build == LFarm;
}
double GetValOfDom(Player * Play){
    int OccSquares = 0;
    for (Domination & Dom : PlayerDom)
        OccSquares += (Dom.Player == Play->PlayerNum && Dom.Influence == MaximumDomination);

    double BValue = 0;
    for (Building * B : Play->Buildings)
        BValue += B->GetCost();

    return BValue/OccSquares;
}
double GetAvBuildIncome(Player * Play){
    double Income = 0;
    for (Building * B : Play->Buildings)
        Income += B->GetCost();
    return Income / Play->Buildings.size();
}
double GetAvValueOfTroops(Player * Play){
    double Value = 0;
    for (Troop * T : Play->Troops)
        Value += T->GetValue();
    return Value / Play->Troops.size();
}
BuildList GetBuildList(vector<Building *> & BList){
    BuildList L;
    for (Building * B : BList)
        L.add(B->Type);
    return L;
}
//Player polymorphic overloads
void SimpleCompPlayer::PlaceNewTroop(Point spot, TInfo::TroopTypes ThingType){
    //whenever it places a new troop, it assigns it to a group of itself
    Player::PlaceNewTroop(spot,ThingType);

    //gets the total value of all groups of each type
    using namespace GroupTypes;
    auto GetBestGType = [&](){
        //adds up the values of the groups you have
        GroupVal TotHaveVal(0, 0, 0, 0);
        int NumOfG = Groups.size();
        for (Group * G : Groups){
            int Val = G->GetValue();
            switch (G->Type){
            case Burn:TotHaveVal.BurnVal += Val; break;
            case Raid:TotHaveVal.RaidVal += Val; break;
            case FullAttack:TotHaveVal.FullAttackVal += Val; break;
            case Defend:TotHaveVal.DefendVal += Val; break;
            }
        }
        //divides the groups you want by the groups you have
        GroupVal Ratio;
        Ratio.BurnVal = TotGroupValue.BurnVal / (TotHaveVal.BurnVal + 0.00001);
        Ratio.RaidVal = TotGroupValue.RaidVal / (TotHaveVal.RaidVal + 0.00001);
        Ratio.FullAttackVal = TotGroupValue.FullAttackVal / (TotHaveVal.FullAttackVal + 0.00001);
        Ratio.DefendVal = TotGroupValue.DefendVal / (TotHaveVal.DefendVal + 0.00001);

        return GetBestType(Ratio);
    };
    GroupType BestT = GetBestGType();
    Group * TroopG = new Group(BestT);
    //initializes the group
    TroopG->Type = BestT;
    //adds the current troop to this group
    TroopG->Troops.push_back(Troops.back());
    //adds it to the list
    Groups.push_back(TroopG);
}
void SimpleCompPlayer::RemoveThingCompletely(BaseType ThingType, int Item){
    Troop * DeleteTroop = Troops[Item];
    Player::RemoveThingCompletely(ThingType, Item);
    if (ThingType != TroopType)
        return;
    for (Group * G : Groups){
        for (int GTNum : range(G->GetSize())){
            if (G->Troops[GTNum] == DeleteTroop){
                G->Troops.erase(G->Troops.begin() + GTNum);
                break;
            }
        }
    }
}
//these functions find the highest value of their type(OutValue), their apropriate refrence numbers(BuildNum) and whether they are avaliable at all or not
Array2d<double> SimpleCompPlayer::GetDefaultTSqVals(Troop * T){
    return GetValuesOfTroopSquares(T, Attach::AttachVals(), Modifier{ 1.0, 1.0, 1.0, 1.0 });
}
double SimpleCompPlayer::GetDefaultBuyValue(Troop * T,const Array2d<double> & Values){
    auto ProtectKey = Protect(PlayerOcc[T->Spot]);
    PlayerOcc[T->Spot] = -1;
    return max(GetPaths(T->Spot, T->MovementPoints, Values).Arr).Val;
}
double SimpleCompPlayer::GetValOfTroopWithAttach(Troop * T, Attach::Attachment Attc){
    unique_ptr<Troop> AttcT(MakeNewTroop(T->ThisType, T->Spot));
    AttcT->Attcs.add(Attc);
    AttcT->ResetStats();
    return GetDefaultBuyValue(AttcT.get(), GetDefaultTSqVals(AttcT.get()));
}
ValInfo<pair<Troop *, Attach::Attachment>> SimpleCompPlayer::FindBestSolierAttachValueAndNum(){
    using namespace Attach;
    //initalises output to appropriate values
    Troop * OutTroop = NULL;
    double OutValue = -100000;
    Attachment OutAttach = NoAttach;
    for (Troop * T : Troops){
        double DefaultVal = GetValOfTroopWithAttach(T, NoAttach);
        for (Attachment A : TInfo::GetAvaliableAtts(T->ThisType)){
            if (CanBuyAttach(A, T)){
                double CurVal = GetValOfTroopWithAttach(T, A) - DefaultVal;
                if (CurVal > OutValue){
                    OutValue = CurVal;
                    OutTroop = T;
                    OutAttach = A;
                }
            }
        }
    }
    return{ OutValue, { OutTroop, OutAttach } };
}
ValInfo<BuildingTypes> SimpleCompPlayer::FindBestBuildType(bool TroopCountNotMet){
    const int CurIncome = GetIncomeFromBuildings();

    int AvEnIncome = 0;
    for(EPlayer * En:Enemies)
        AvEnIncome += En->GetIncomeFromBuildings();

    AvEnIncome /= Enemies.size();

    const double GoalIncomeAdvantage = 0.2;//arbitrary constants
    const double ExpectedNumOfTurns = 35;
    double IncomeVal = ExpectedNumOfTurns * (AvEnIncome * GoalIncomeAdvantage) / (CurIncome + 0.00001);

    double ThisValOfDom = GetValOfDom(this);

    int ThreatToBuildingsVal = 0;
    for(Building * B:Buildings){
        for(Point & P:B->Place){
            if(Threat[P].BThreat > 0)
                ThreatToBuildingsVal += B->GetCost() / B->GetNumOfSquares();
        }
    }
    BuildList BTypes = GetBuildList(Buildings);

    const int TCCount = CountBuildings(TrainCenter);
    const int CFCount = CountBuildings(CataFact);

    BuildVals Value;
    for (BuildingTypes BType : IncomeBuilds){
        Value[BType] = (IncomeVal * GetBuildingIncome(BType)) - (ThisValOfDom * GetBuildingSize(BType));
    }
    for (BuildingTypes BType : TrainBuilds){//deals with TrainBuilds
        if (!BTypes.has(BType) || SoughtAfter.has(BType))
            Value[BType] = GetValOfTroopWithAttach(
                unique_ptr<Troop>(MakeNewTroop(GetTroopType(BType), FindBestBuildSpot(BType).first)).get()
                , Attach::NoAttach);
        else
            Value[BType] = 0;
    }
    for (BuildingTypes BType : AttachBuilds){
        double Sum = 0;
        if (!BTypes.has(BType) || SoughtAfter.has(BType))
        for (Troop * T : Troops){
            double DefaultTVal = GetValOfTroopWithAttach(T, Attach::NoAttach);
            for (Attach::Attachment A : BuildAttachMake(BType)){
                if (T->CanHaveAttach(A)){
                    Sum += max(0.0,GetValOfTroopWithAttach(T, A) - DefaultTVal);
                }
            }
        }
        Value[BType] = Sum;
    }
    auto MaxIterator = Value.begin();
    return ValInfo<BuildingTypes>(MaxIterator->first,MaxIterator->second);
}
pair<Point,Point> SimpleCompPlayer::FindBestBuildSpot(BuildingTypes Build){
    Array2d<double> BoardValues(0);

    int XAdd, YAdd;
    int Size = GetBuildingSize(Build);
    if (Size == 4){
        XAdd = 1;
        YAdd = 1;
    }//add more sizes of buildings here

    //buildings don't want to be threatened at all
    int BuildVal = GetBuildingCost(Build);

    //militart buildings like combat
    int CombatGoodness;
    if (BuildIsMilitary(Build))
        CombatGoodness = 1;
    else if (BuildIsCivilian(Build))//economic buildings hate combat
        CombatGoodness = -1;

    for (Point BuildP:BoardIterate()){
        BoardValues[BuildP] -= Threat[BuildP].BThreat * BuildVal;
        for (Vector & EV : EnemyPaths){
            //whether it is in the path of nearest attack
            if (DistanceToVector(BuildP, EV) == 0)
                BoardValues[BuildP] += 10 * CombatGoodness;

            //distance to enemies minimized
            BoardValues[BuildP] += 1000 * DisConvert(GetBoardDistance(BuildP, EV.Dest)) * CombatGoodness;
        }
        //todo:find something to replace this!
        //BoardValues[BuildP] += GroupValue[BuildP].GetTotVal() / 4.0 * CombatGoodness;
    }
    //adjusts whether they want to be around other buildings
    for (Building * B : Buildings){
        BuildingTypes BT = B->Type;
        int LikesNeighbors;
        if (BuildIsMilitary(BT))
            LikesNeighbors = -2;//crouded military buildings are bad
        else if (BuildIsCivilian(BT))
            LikesNeighbors = 2;//crouded civilian buildings are good

        for (Point & BP : B->Place){
            for (Point IterP:SquareIterate(BP, 1)){
                BoardValues[IterP] += LikesNeighbors;//remember that these values overlap, so the effect is higher than 2!
            }
        }
    }

    double BestVal = -10000000.0;
    Point BestP = CreatePoint(0,0);
    for (Point P:BoardIterate()){
        if (P.X + XAdd >= BoardSizeX || P.Y + YAdd >= BoardSizeY)
            continue;
        double CurVal = 0;
        bool CanBuild = true;
        //iterate over the spot the building needs to be built in
        for (Point BP: RectIterate(P.X, P.Y, P.X + XAdd, P.Y + YAdd)){
            CurVal += BoardValues[BP];
            if (!BlankPoint(BP) || PlayerDom[BP].Influence != MaximumDomination || PlayerDom[BP].Player != PlayerNum)
                CanBuild = false;
        }
        if (!CanBuild)
            continue;
        if (CurVal > BestVal){
            BestVal = CurVal;
            BestP = P;
        }
    }
    return pair<Point, Point>(BestP, CreatePoint(BestP.X + XAdd, BestP.Y + YAdd));
}
TInfo::TroopVals SimpleCompPlayer::FindTroopValues() {

    auto CanBuy = [&](TInfo::TroopTypes TType){
        BuildingTypes NeededBuild = GetBuildType(TType);
        for (Building * B : Buildings){
            if ((B->Type == NeededBuild) && B->BuyAmmount > 0){
                return true;
            }
        }
        return false;
    };
    using namespace TInfo;
    //finds the the value of the troops in an objective way, by simple
    //looking for the best spot they could be and recording the value of it being there.
    TroopVals Vals;
    Vals[CatapultType] = FindBestTroopSpot(CatapultType).first;
    Vals[SoldierType] = FindBestTroopSpot(SoldierType).first;

    return Vals;
}
ValInfo<TInfo::TroopTypes> SimpleCompPlayer::FindBestTroopValue(){
    TInfo::TroopVals TVals = FindTroopValues();
    auto Pair = TVals.begin();
    return{ Pair->first, Pair->second };
}
triple<double,Point,Building *> SimpleCompPlayer::FindBestTroopSpot(TInfo::TroopTypes TType){
    Building * BestBuild = NULL;
    BuildingTypes NeedBuild = GetBuildType(TType);
    unique_ptr<Troop> SimTroop(MakeNewTroop(TType, CreatePoint(0, 0)));
    SimTroop->ResetStats();
    Array2d<double> TVals = GetValuesOfTroopSquares(SimTroop.get(), TInfo::GetDefaultAttachVals(SimTroop->ThisType), Modifier{ 1.0,1.0, 1.0, 1.0 });
    PointVal Best;
    for (Building * B : Buildings){
        if (B->Type == NeedBuild){
            B->IterOverBuildFromDis(PlayerOcc, 1, [&](Point PosP){
                double ThisVal = GetPaths(PosP, SimTroop->MovementPoints, TVals)[PosP].Val;//Performance warning!!!!!
                if (ThisVal > Best.Val){
                    Best = PointVal(ThisVal, PosP);
                    BestBuild = B;
                }
            });
        }
    }
    return{ Best.Val,Best.Info, BestBuild };
}
void SimpleCompPlayer::InteleBuild(){
    /*standard range of values for intelebuild
    Numbers go from 0 as worst to +?? as best
    Attachments =
    Troops =
    Building =
    */
    //takes a whole bunch of times recalculating values
    BuildList NewSoughtAfter;
    BuildList NewRanOut;
    while(true){
        CalcPowerAndCenter();

        auto AttachInfo = FindBestSolierAttachValueAndNum();
        auto BuildInfo = FindBestBuildType(true);//replace constant with real value when fully implemented
        auto TroopInfo = FindBestTroopValue();
        TroopInfo.Val *= 5;//remove!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1

        double MaxVal = max(max(AttachInfo.Val, TroopInfo.Val), BuildInfo.Val);
        if (MaxVal < 0.01)
            break;
        if (MaxVal == AttachInfo.Val){//attach value is best
            if (!BuyAttach(AttachInfo.Info.second, AttachInfo.Info.first))
                break;
        }
        else if (MaxVal == TroopInfo.Val){//troop value is best
            auto TroopSpot = FindBestTroopSpot(TroopInfo.Info);
            Point TP = TroopSpot.second;
            Building * TrainB = TroopSpot.third;
            if (TrainB->BuyAmmount > 0){
                if (BlankPoint(TP) && TInfo::GetTroopCost(TroopInfo.Info) < Money){
                    TrainB->BuyAmmount -= 1;
                    PlaceNewTroop(TP, TroopInfo.Info);
                }
                else
                    break;
            }
            else{
                BuildingTypes BTy = TrainB->Type;
                if (SoughtAfter.has(BTy) || RanOut.has(BTy))
                    NewSoughtAfter.add(BTy);
                else
                    NewRanOut.add(BTy);

                break;
            }
        }
        else if (MaxVal == BuildInfo.Val){ //build value is best
            auto BSpot = FindBestBuildSpot(BuildInfo.Info);
            if (!PlaceNewBuilding(BuildInfo.Info, BSpot.first.X, BSpot.first.Y, BSpot.second.X, BSpot.second.Y))
                break;
        }
        else
            break;

    }
    SoughtAfter = NewSoughtAfter;
    RanOut = NewRanOut;

    CalcPowerAndCenter();
}
