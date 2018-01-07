#include "stdafx.h"
#include "RealPlayer.h"
RealPlayer::RealPlayer(void):Player(){
    Type = Real;
    SelectedThing = NoSelect;
    Selected1 = { -1, -1 };
    Selected2 = { -1, -1 };
}
RealPlayer::~RealPlayer(void){
}
bool RealPlayer::BuyAttach(int TroopNum){
    Troop * T = Troops[TroopNum];
    TInfo::TroopTypes Type = T->ThisType;
    Attach::Attachment Attc = BuyerDialogue(Type);
    return Player::BuyAttach(Attc,T);
}
void RealPlayer::SelectThing(SelectTypes Thing){
    // makes the player "pick up" a potencial building
    // player does not pay for building, but it does chech if they have enough
    // match picture with selected building
    if (MiniTurn != 0)
        return;
    BuildingTypes Build = SelectToBuild(Thing);
    TInfo::TroopTypes TType = SelectToTroop(Thing);
    int Cost = 0;
    if (Build != NoBuild){
        Cost = GetBuildingCost(Build);
    }
    else if (TType != TInfo::NoType){
        Cost = TInfo::GetTroopCost(TType);
    }
    if (Cost > Money)
        MakeDialogue(L"Sorry, but you do not have enough money to purchace this", L"Not Enough Money");
    else
        SelectedThing = Thing;
}
void RealPlayer::RightClick(int xbox,int ybox){
    Point ClickP = CreatePoint(xbox, ybox);
    if(MiniTurn == 0){//Building Part of turn
        if(TPlayerMap[ClickP] == BuildingType){
            if(DemolishDialogue()){
                RemoveThingCompletely(BuildingType,NPlayerMap[ClickP]);
            }
        }
        if(BlankPoint(ClickP) && SelectedThing != NoSelect){
            if(Selected1.X < 0){
                Selected1 = ClickP;
            }
            else if(Selected2.Y < 0){
                if (SelectToBuild(SelectedThing) != NoBuild)
                    PlaceNewBuilding(SelectToBuild(SelectedThing), Selected1.X, Selected1.Y, xbox, ybox);
                else if (SelectToTroop(SelectedThing) == TInfo::WallType)
                    PlaceWall(Selected1, ClickP);
                SelectedThing = NoSelect;

                Selected1 = ClearPoint();
            }
        }
    }
    else if(MiniTurn == 1){//moving part of turn

    }
    else if(MiniTurn == 2){//attacking part of turn

    }
}
void RealPlayer::LeftClick(int xbox,int ybox) {
    Point ClickP = CreatePoint(xbox,ybox);
    if(MiniTurn == 0){//Building Part of turn
        //acts as a way to clear the selection by clicking somewhere else
        if(SelectedThing != NoSelect){
            Selected1 = ClearPoint();
            SelectedThing = NoSelect;
            return;
        }
        //allows the player to click to center the troop dispach, and finalize the purchase
        //is all about buildings, nothing else should be able to be selected in Miniturn 0
        //selectedsquare1 is the place where the person selectes the building that will produce troops
        if(Selected1.X >= 0){
            int BuildNum = NPlayerMap[Selected1];
            BuildingTypes Build = Buildings[BuildNum]->Type;
            const int MaxBuyAmmount = 10;
            for (int m = 0; m <= MaxBuyAmmount && NumOfBuyTroops > 0; m++){
                Point CurPoint;
                if (TroopBuyPutter(Buildings[BuildNum], CurPoint, ClickP)){
                    Buildings[BuildNum]->BuyAmmount -= 1;
                    PlaceNewTroop(CurPoint, GetTroopType(Build));
                    NumOfBuyTroops--;
                }
                if (m == MaxBuyAmmount){
                    MakeDialogue(L"Not enough room for all troops.",L"Error");
                    break;
                }
            }
            Selected1 = ClearPoint();
        }
        int Num = NPlayerMap[ClickP];
        BaseType ThingType = TPlayerMap[ClickP];
        if(ThingType == BuildingType){
            //controls the selection of the building of troops
            BuildingTypes Build = Buildings[Num]->Type;
            using namespace TInfo;
            if(Build == TrainCenter){
                NumOfBuyTroops = NumOfSolidersDialogue();
                if(Buildings[Num]->BuyAmmount < NumOfBuyTroops){
                    MakeDialogue(L"This building can only buy 5 soldiers a turn.",L"Error");
                    NumOfBuyTroops = Buildings[Num]->BuyAmmount;
                }
                if (NumOfBuyTroops * GetTroopCost(SoldierType) > Money){
                    MakeDialogue(L"Not enough money for all soldiers.",L"Error");
                    NumOfBuyTroops = Money / GetTroopCost(SoldierType);
                }
                if(NumOfBuyTroops != 0)
                    Selected1 = ClickP;
            }
            else if(Build == CataFact){
                NumOfBuyTroops = NumOfCatasDialogue();
                if(Buildings[Num]->BuyAmmount < NumOfBuyTroops){
                    MakeDialogue(L"This building can only buy one catapult a turn.",L"Error");
                    NumOfBuyTroops = Buildings[Num]->BuyAmmount;
                }
                if (NumOfBuyTroops * GetTroopCost(CatapultType) > Money){
                    MakeDialogue(L"Not enough money for all catapults.",L"Canceled");
                    NumOfBuyTroops = Money / GetTroopCost(CatapultType);
                }
                if(NumOfBuyTroops != 0)
                    Selected1 = ClickP;
            }
        }
        else if(ThingType == TroopType){
            BuyAttach(Num);
        }
    }
    else if(MiniTurn == 1){//moving part of turn
        if(Selected1.X >= 0){
            if(!BlankPoint(ClickP)){
                Selected1 = ClearPoint();
                return;
            }
            int TroopNum = NPlayerMap[Selected1];
            BaseType TType = TPlayerMap[Selected1];
            if(TType == TroopType){
                MoveTroop(TroopNum,ClickP);
                Selected1 = ClearPoint();
                return;
            }
        }
        int Num = NPlayerMap[ClickP];
        BaseType ThingType = TPlayerMap[ClickP];
        if(ThingType == TroopType){
            if(Troops[Num]->MovementPoints == 0){
                MakeDialogue(L"This unit has no movement points left",L"Error");
            }
            else{
                Selected1 = ClickP;
            }
        }
    }
    else if(MiniTurn == 2){//attacking part of turn
        if(Selected1.X >= 0){
            int TroopNum = NPlayerMap[Selected1];
            BaseType TType = TPlayerMap[Selected1];
            if(TType == TroopType){
                if(Troops[TroopNum]->ExploRange == 1){
                    if(AttemptAttackOnOthers(TroopNum,ClickP))
                        AttackLocation(ClickP,Troops[TroopNum]);
                }
                else{
                    Selected2 = ClickP;
                    return;
                }
            }
        }
        if(Selected2.X >= 0){
            //Todo: implement multiple explosion squares
        }
        int Num = NPlayerMap[ClickP];
        BaseType ThingType = TPlayerMap[ClickP];
        if(ThingType == TroopType){
            if(Troops[Num]->HaveAttacked == true){
                MakeDialogue(L"This unit has already attacked this turn",L"Error");
            }
            else{
                Selected1 = ClickP;
            }
        }
    }
}
void RealPlayer::DrawInterface(){
    DrawMoney(Money);
    ShadeSelectedBuilding(SelectedThing);
    Color CurColor;
    if(MiniTurn == 0)
        CurColor = Blue;
    if(MiniTurn == 1)
        CurColor = Green;
    if(MiniTurn == 2)
        CurColor = Red;
    if (OnBoard(Selected1))
        DrawColorSquare(CurColor,Selected1.X,Selected1.Y,0.5);
}
bool RealPlayer::ChangeTurn(){
    Selected1 = ClearPoint();
    Selected2 = ClearPoint();
    SelectedThing = NoSelect;
    return Player::ChangeTurn();
}
bool RealPlayer::TroopBuyPutter(Building * B, Point & OutSquare, Point GetCloseTo){
    double SmallestDistance = 10000;
    Point SmallestPoint;
    B->IterOverBuildFromDis(PlayerOcc,1, [&](Point TroopP){
        double Distance = TrueDistance(TroopP, GetCloseTo);
        if (Distance < SmallestDistance){
            SmallestDistance = Distance;
            SmallestPoint = TroopP;
        }
    });
    OutSquare = SmallestPoint;
    if (SmallestDistance > 1000){
        return false;
    }
    else {
        return true;
    }
}
void RealPlayer::PlaceWall(Point P1, Point P2){
    int XDis = abs(P1.X - P2.X);//finds the actual distance between the points
    int YDis = abs(P1.Y - P2.Y);
    if ((XDis != 0) || (YDis != 0))
        return;
    int Squares = max(XDis, YDis) + 1;
    int Cost = Squares * TInfo::GetTroopCost(TInfo::WallType);
    if (Cost > Money)
        MakeDialogue(L"Not enough money!",L"Error!");

    Point AddP = CreatePoint(XDis > YDis, YDis > XDis);
    Point LowP = P1.X < P2.X ? P1 : P1.Y < P2.Y ? P1 : P2;
    Point HighP = P1 + P2 - LowP;
    //checks to see if the spots are open
    for (Point P = LowP; P != HighP; P += AddP){
        if(!BlankPoint(P) || PlayerDom[P].Player != PlayerNum || PlayerDom[P].Influence < MaximumDomination)
            return;
    }
    //has passed all checks
    for (Point P = LowP; P != HighP; P += AddP){
        Troops.push_back(MakeNewTroop(TInfo::WallType, P));
        int TroopNum = Troops.size() - 1;
        reinterpret_cast<WallClass *>(Troops[TroopNum])->Vertical = (AddP == CreatePoint(0, 1));

        PlayerOcc[P] = PlayerNum;
        TPlayerMap[P] = BuildingType;
        NPlayerMap[P] = TroopNum;
    }
    Money -= Cost;
}
