
#include "stdafx.h"
#include "Player.h"
TInfo::TroopTypes GetTroopType(BuildingTypes Build){
    switch (Build){
    case TrainCenter:return TInfo::SoldierType;
    case CataFact:return TInfo::CatapultType;
    }
}
BuildingTypes GetBuildType(TInfo::TroopTypes TType){
    switch (TType){
    case TInfo::SoldierType:return TrainCenter;
    case TInfo::CatapultType:return CataFact;
    default: return NoBuild;
    }
}
BuildingTypes AttBuildReqs(Attach::Attachment Attach){
    using namespace Attach;
    switch(Attach){
    case Torch:return TorchFactory;
    case Horse:return Stable;
    case Armor: return Armory;
    case BA: return BA_Factory;
    case CataHorse: return Stable;
    }
}
Attach::AttachList BuildAttachMake(BuildingTypes Build){
    using namespace Attach;
    AttachList A;
    switch (Build){
    case TorchFactory: A.add(Torch); break;
    case Stable:
        A.add(Horse);
        A.add(CataHorse); break;
    case Armory: A.add(Armor); break;
    case BA_Factory: A.add(BA); break;
    }
    return A;
}
Troop * MakeNewTroop(TInfo::TroopTypes TType, Point AtSpot){
    Troop * NewT = NULL;
    switch (TType){
    case TInfo::SoldierType:
        NewT = new Soldier(AtSpot);
        break;
    case TInfo::CatapultType:
        NewT = new Catapult(AtSpot);
        break;
    case TInfo::WallType:
        NewT = new WallClass(AtSpot);
        break;
    }
    return NewT;
}
Troop * MakeNewTroop(Troop * Old){
    Troop * NewT = NULL;
    switch (Old->ThisType){
    case TInfo::SoldierType:
        NewT = new Soldier(*reinterpret_cast<Soldier *>(Old));
        break;
    case TInfo::CatapultType:
        NewT = new Catapult(*reinterpret_cast<Catapult *>(Old));
        break;
    case TInfo::WallType:
        NewT = new WallClass(*reinterpret_cast<WallClass *>(Old));
        break;
    }
    return NewT;
}
Troop * MakeNewTroop(TInfo::TroopTypes TType){
    return MakeNewTroop(TType, CreatePoint(0, 0));
}
int GetDominationOfTroop(int Range, int Distance){
    return (MaximumDomination * (Range + 1 - Distance)) / (Range + 1);
}
Player::Player(void){
    StartNew();
}
Player::~Player(void){
    ClearData();
}
Player::Player(Player * CopyFrom){
    DeepCopy(CopyFrom);
}
void Player::DeepCopy(Player * CopyFrom){
    PlayerNum = CopyFrom->PlayerNum;
    Type = CopyFrom->Type;
    Money = CopyFrom->Money;
    MiniTurn = CopyFrom->MiniTurn;
    TeamNum = CopyFrom->TeamNum;
    CurrentTurn = CopyFrom->CurrentTurn;
    TPlayerMap = CopyFrom->TPlayerMap;
    NPlayerMap = CopyFrom->NPlayerMap;

    Troops.resize(CopyFrom->Troops.size());
    Buildings.resize(CopyFrom->Buildings.size());
    //copies the old buildings and troops into a new memory location
    for (int t = 0; t < CopyFrom->Buildings.size(); t++){
        this->Buildings[t] = new Building(*CopyFrom->Buildings[t]);
    }
    for (int t = 0; t < CopyFrom->Troops.size(); t++){
        this->Troops[t] = MakeNewTroop(CopyFrom->Troops[t]);
    }
}
void Player::Save(ofstream & File){
    uint Var = Troops.size();
    File.write((char *)(&Var),sizeof(Var));
    Var = Buildings.size();
    File.write((char *)(&Var),sizeof(Var));
    File.write((char *)(&Type),sizeof(Type));
    File.write((char *)(&Money),sizeof(Money));
    File.write((char *)(&TeamNum),sizeof(TeamNum));
    File.write((char *)(&PlayerNum),sizeof(PlayerNum));
    File.write((char *)(&MiniTurn),sizeof(MiniTurn));
    File.write((char *)(&CurrentTurn),sizeof(CurrentTurn));
    File.write((char*)(&NPlayerMap),NPlayerMap.Size());
    File.write((char*)(&TPlayerMap), TPlayerMap.Size());

    for(Troop * T:Troops){
        File.write((char *)(&T->ThisType), sizeof(T->ThisType));
        T->Save(File);
    }
    for(Building * B:Buildings){
        B->Save(File);
    }
}
void Player::Load(ifstream & File){
    ClearData();
    //start loading here
    uint Var;
    File.read((char *)(&Var),sizeof(Var));
    Troops.resize(Var);
    File.read((char *)(&Var),sizeof(Var));
    Buildings.resize(Var);
    File.read((char *)(&Type),sizeof(Type));
    File.read((char *)(&Money),sizeof(Money));
    File.read((char *)(&TeamNum),sizeof(TeamNum));
    File.read((char *)(&PlayerNum),sizeof(PlayerNum));
    File.read((char *)(&MiniTurn),sizeof(MiniTurn));
    File.read((char *)(&CurrentTurn),sizeof(CurrentTurn));
    File.read((char*)(&NPlayerMap),NPlayerMap.Size());
    File.read((char*)(&TPlayerMap), TPlayerMap.Size());

    for(Troop *& T:Troops){
        TInfo::TroopTypes ThisType;
        File.read((char *)(&ThisType),sizeof(ThisType));

        T = MakeNewTroop(ThisType);

        T->Load(File);
    }
    for(Building *& B:Buildings){
        B = new Building;//info will be loaded
        B->Load(File);
    }
}
void Player::DrawInterface(){
    DrawMoney(Money);
}
void Player::Draw(){
    for(Troop * T:Troops)
        T->Draw();

    for(Building * B:Buildings)
        B->Draw();
}
void Player::ClearData(){
    for(Troop * T:Troops)
        delete T;

    for(Building * B:Buildings)
        delete B;

    Troops.clear();
    Buildings.clear();
}
void Player::StartNew(){
    ClearData();

    Money = MoneyResetVal;
    MiniTurn = 0;
    TPlayerMap.Init(Nothing);
}
void Player::RemoveThingCompletely(BaseType ThingType,int Item){
    auto KnockDown = [&]() {
        for (Point P : BoardIterate()){
            if(TPlayerMap[P] == ThingType){
                if(NPlayerMap[P] > Item)
                    NPlayerMap[P] -= 1;//knocks down the other coords to -
            }
        }
    };
    if(ThingType == TroopType){
        Point P = Troops[Item]->Spot;
        PlayerOcc[P] = -1;
        TPlayerMap[P] = Nothing;
        delete Troops[Item];
        Troops.erase(Troops.begin() + Item);
        KnockDown();
    }
    else if(ThingType == BuildingType){
        for (Point P : Buildings[Item]->Place){
            PlayerOcc[P] = -1;
            TPlayerMap[P] = Nothing;
        }
        delete Buildings[Item];
        Buildings.erase(Buildings.begin() + Item);
        KnockDown();
    }
}
void Player::PlaceNewTroop(Point spot,TInfo::TroopTypes Type){
    int TroopNum = Troops.size();
    //actually creates the soldier
    Troops.push_back(MakeNewTroop(Type, spot));

    TPlayerMap[spot] = TroopType;
    NPlayerMap[spot] = TroopNum;
    PlayerOcc[spot] = PlayerNum;
    // real cost
    Money -= GetTroopCost(Type);
}
bool Player::PlaceNewBuilding(BuildingTypes Build,int x1,int y1,int x2,int y2){
    auto FindGreatestFactor = [](int x){
        for (int i = x - 1; i > 0; i--)
            for (int j = 1; j <= i; j++)
                if (i*j == x)
                    return i;
    };
    const int BShape = GetBuildingSize(Build);
    int BCost = GetBuildingCost(Build);
    if (BCost > Money)
        return false;
    int XDis = abs(x1 - x2);//finds the actual distance between the points
    int YDis = abs(y1 - y2);

    if ((XDis + 1) * (YDis + 1) != BShape)//it is the wrong size
        return false;

    int OneSide = FindGreatestFactor(BShape);//finds the needed distance between the points
    int OtherSide = BShape / OneSide;
    OneSide -= 1;
    OtherSide -= 1;

    //makes sure that the distances line up with the right dimentions
    if (max(XDis, YDis) != max(OneSide, OtherSide))//yes, this works because the multiplication between the two is the same
        return false;
    int LowX = min(x1,x2);
    int LowY = min(y1,y2);

    int HighX = max(x1,x2);
    int HighY = max(y1,y2);

    vector<Point> BPs;
    //checks to see if the spots are open
    for (Point P : RectIterate(LowX,LowY,HighX,HighY)){
        BPs.push_back(P);
        if(!BlankPoint(P) || PlayerDom[P].Player != PlayerNum || PlayerDom[P].Influence < MaximumDomination)
            return false;
    }
    Buildings.push_back(new Building(Build,x1,y1,x2,y2));
    int BuildNum = Buildings.size() - 1;
    for (Point BP : BPs){
        PlayerOcc[BP] = PlayerNum;
        TPlayerMap[BP] = BuildingType;
        NPlayerMap[BP] = BuildNum;
    }

    Money -= BCost;
    return true;
}
bool Player::CanBuyAttach(Attach::Attachment Attach, Troop * T){
    if(T->HasAttach(Attach) == true){
        MakeDialogue(L"This troop already this attachment!",L"Error");
        return false;
    }
    int MaxAttNum = GetBuyAmmounts(AttBuildReqs(Attach),T->Spot);
    if(MaxAttNum <= 0){
        MakeDialogue(L"You must be near an attachment center and each attachment production center can only produce 5 attachments per turn.",L"Error");
        return false;
    }
    //all checks have been passed
    return true;
}
bool Player::BuyAttach(Attach::Attachment Attach,Troop * T){
    int Price = Attach::GetAttachCost(Attach);
    if (Money < Price)
        MakeDialogue(L"You don't have enough money", L"Error");
    else if (T->HasAttach(Attach))
        MakeDialogue(L"The troop already has this attachment", L"Error");
    else if (!T->CanHaveAttach(Attach))
        MakeDialogue(L"This troop cannot have this attachment: This error should not actually be possible, contact me", L"Error");
    else{
        //all checks have been passed, and it procedes to finish the purchase
        SubtractBuyAmmountByOne(AttBuildReqs(Attach),T->Spot);
        T->Attcs.add(Attach);
        Money -= Price;
        return true;
    }
    return false;
}
int Player::GetIncomeFromBuildings(){
    int Income = 0;
    for(int g = 0; g < Buildings.size(); g++){
        Income += Buildings[g]->GetIncome();
    }
    return Income;
}
void Player::ResetStats(){
    for(Troop * T:Troops)
        T->ResetStats();
    for(Building * B:Buildings)
        B->ResetStats();
}
bool Player::ChangeTurn(){
    ResetStats();
    if(MiniTurn < 2){
        MiniTurn += 1;
        return false;
    }
    else{
        MiniTurn = 0;
        return true;
    }
}
BaseType Player::GetMyBType(Point P){
    return TPlayerMap[P];
}
Troop * Player::GetMyTroop(Point P){
    if (GetMyBType(P) == TroopType)
        return Troops[NPlayerMap[P]];
    else
        return NULL;
};
Building * Player::GetMyBuild(Point P){
    if (GetMyBType(P) == BuildingType)
        return Buildings[NPlayerMap[P]];
    else
        return NULL;
};
bool Player::AttemptAttackOnOthers(int TroopNum,Point Loc){
    if(BlankPoint(Loc) || PlayerOcc[Loc.X][Loc.Y] == PlayerNum)
        return false;
    Troop * CurTroop = Troops[TroopNum];
    if(CurTroop->HaveAttacked)
        return false;
    if(!CheckAttack(CurTroop->Spot,Loc,CurTroop->Range))
        return false;
    if(!AttemptAttack(Loc,CurTroop))
        return false;

    return true;
}
int Player::GetBuyAmmounts(BuildingTypes Build,Point GetSpot){
    int Sum = 0;
    for(Building * B:Buildings){
        if (B->Type == Build){
            bool Worked = false;
            for (Point & P : B->Place)
                if (GetBoardDistance(P, GetSpot) <= AttachBuyRange)
                    Worked = true;
            if(Worked)
                Sum += B->BuyAmmount;
        }
    }
    return Sum;
}
void Player::SubtractBuyAmmountByOne(BuildingTypes Build,Point BuySpot){
    for (Building * B:Buildings){
        if(B->Type == Build && B->BuyAmmount > 0){
            bool Worked = false;
            for(Point & P :B->Place){
                if (GetBoardDistance(P, BuySpot) <= AttachBuyRange){
                    B->BuyAmmount -= 1;
                    return;
                }
            }
        }
    }
}
bool Player::MoveTroop(int TroopNum,Point FinalPoint){
    Troop * CurTroop = Troops[TroopNum];
    int MovePoints = CurTroop->MovementPoints;
    Point InitPoint = CurTroop->Spot;
    if (InitPoint == FinalPoint){
        return true;
    }
    if(GetBoardDistance(InitPoint,FinalPoint) > MovePoints){
        MakeDialogue(L"Too far away",L"Error");
        return false;
    }
    if(CheckMovement(InitPoint,FinalPoint,MovePoints) == true){
        CurTroop->MovementPoints -= GetPointsForMove(InitPoint,FinalPoint,MovePoints);

        CurTroop->Spot = FinalPoint;

        TPlayerMap[InitPoint] = Nothing;
        TPlayerMap[FinalPoint] = TroopType;

        PlayerOcc[InitPoint] = -1;
        PlayerOcc[FinalPoint] = PlayerNum;

        NPlayerMap[FinalPoint] = TroopNum;
        return true;
    }
    else{
        MakeDialogue(L"No clear path. Try again with a shorter, more clear path",L"Error");
        return false;
    }
}
void Player::PlayerAttackLocation(Point Loc,Troop * AttackingTroop){
    if (!AttemptAttackOnSelf(Loc, AttackingTroop))
        return;
    BaseType ThingType = TPlayerMap[Loc];
    int ThingNum = NPlayerMap[Loc];
    if(ThingType == TroopType){
        if(Troops[ThingNum]->DestroyedAfterAttack(AttackingTroop))//if it is completely destroyed, rather than just damaged
            RemoveThingCompletely(ThingType,ThingNum);
    }
    else if(ThingType == BuildingType){
        if (Buildings[ThingNum]->DestroyedAfterAttack(Loc, AttackingTroop))//if it is completely destroyed, rather than just damaged
            RemoveThingCompletely(ThingType, ThingNum);
        //in either case, the building is cut down
        TPlayerMap[Loc] = Nothing;
        PlayerOcc[Loc] = -1;
    }
}
bool Player::AttemptAttackOnSelf(Point Loc,Troop * AttackingTroop){
    BaseType ThingType = TPlayerMap[Loc];
    int ThingNum = NPlayerMap[Loc];
    if(ThingType == TroopType){
        return Troops[ThingNum]->AttemptAttack(AttackingTroop);//if it can be attacked by the other players thingy
    }
    else if(ThingType == BuildingType){
        return Buildings[ThingNum]->AttemptAttack(AttackingTroop);//if it can be attacked by the other players thingy
    }
}
void Player::AssertDommination(){
    for (Troop * T: Troops){
        Point CenterP = T->Spot;
        int Range = T->Range;
        for(Point P:SquareIterate(CenterP,Range)){
            int ValChange = GetDominationOfTroop(T->Range,GetBoardDistance(T->Spot,P));
            if(PlayerDom[P].Player != PlayerNum){
                ValChange *= 2;//double the effect for decreasing other peoples stuff
                PlayerDom[P].Influence -= ValChange;
                if(PlayerDom[P].Influence < 0){//shifts over to you
                    PlayerDom[P].Player = PlayerNum;
                    PlayerDom[P].Influence = abs(PlayerDom[P].Influence);
                }
            }
            else
                PlayerDom[P].Influence += ValChange;
        }
    }
    for (Building * B:Buildings){
        for(Point P: B->Place){
            PlayerDom[P].Player = PlayerNum;
            PlayerDom[P].Influence = MaximumDomination;
        }
    }
    for (Domination & Dom : PlayerDom){
        if (Dom.Influence > MaximumDomination)
            Dom.Influence = MaximumDomination;
    }

}
bool Player::CheckAngledRange(Point Start,Point Finish){
    int StartX = Start.X, StartY = Start.Y, FinishX = Finish.X, FinishY = Finish.Y;
    int XDirection, YDirection;
    double Corner1[3][2], Corner2[3][2];//[three differnt coords][x,y]
    //you don't have to worry about equal to becasue that won't show up in this function
    if (StartX > FinishX){
        Corner1[0][0] = StartX;
        Corner1[1][0] = StartX;
        Corner1[2][0] = StartX + 0.5;

        Corner2[0][0] = FinishX + 1;
        Corner2[1][0] = FinishX + 1;
        Corner2[2][0] = FinishX + 0.5;

        XDirection = -1;
    }
    else{//(StartX < FinishX)
        Corner1[0][0] = StartX + 1;
        Corner1[1][0] = StartX + 1;
        Corner1[2][0] = StartX + 0.5;

        Corner2[0][0] = FinishX;
        Corner2[1][0] = FinishX;
        Corner2[2][0] = FinishX + 0.5;

        XDirection = 1;
    }
    if (StartY > FinishY){
        Corner1[0][1] = StartY;
        Corner1[1][1] = StartY + 0.5;
        Corner1[2][1] = StartY;

        Corner2[0][1] = FinishY + 1;
        Corner2[1][1] = FinishY + 0.5;
        Corner2[2][1] = FinishY + 1;

        YDirection = -1;
    }
    else{//(StartY < FinishY)
        Corner1[0][1] = StartY + 1;
        Corner1[1][1] = StartY + 0.5;
        Corner1[2][1] = StartY + 1;

        Corner2[0][1] = FinishY;
        Corner2[1][1] = FinishY + 0.5;
        Corner2[2][1] = FinishY;

        YDirection = 1;
    }
    int Ratio;
    int var;
    int X, Y;
    auto CheckSquare = [&]() -> bool {
        int NewRatio = (Y - FinishY + 0.0) / (X - FinishX + 0.0) + 0.99999;
        if (NewRatio == Ratio)
            if (BlankSpot(X, Y))
                return true;
        return false;
    };
    for (int w = 0; w < 3; w++){
        for (int q = 0; q < 3; q++){
            Ratio = (Corner1[w][1] - Corner2[q][1]) / (Corner1[w][0] - Corner2[q][0]) + 0.99999;
            var = 0;
            X = StartX;
            Y = StartY;
            while (true){
                X += XDirection;
                if (!CheckSquare()){
                    if (X == FinishX && Y == FinishY)
                        return true;
                    X -= XDirection;
                }
                else
                    continue;
                Y += YDirection;
                if (!CheckSquare()){
                    if (X == FinishX && Y == FinishY)
                        return true;
                    Y -= YDirection;
                }
                else
                    continue;
                X += XDirection;
                Y += YDirection;
                if (!CheckSquare()){
                    if (X == FinishX && Y == FinishY)
                        return true;
                    X -= XDirection;
                    Y -= YDirection;
                }
                else
                    continue;
                //if none of those worked, then get out of there and try a different approach
                break;
            }
        }
    }
    return false;
}
bool Player::CheckAttack(Point From, Point To, int Range){
    int x1 = From.X, y1 = From.Y, x2 = To.X, y2 = To.Y;

    int xdistance = x1 - x2;
    int ydistance = y1 - y2;
    if (abs(xdistance) > Range || abs(ydistance) > Range)
        return false;
    int xdirection;
    int ydirection;
    if (xdistance < 0){//absolute values distance and makes direction either negative or positive
        xdirection = -1;
        xdistance = 0 - xdistance;
    }
    else
        xdirection = 1;

    if (ydistance < 0){
        ydirection = -1;
        ydistance = 0 - ydistance;
    }
    else
        ydirection = 1;
    //these vareables are necessary for the recursive analyses
    int CurrentCoordX = x1;
    int CurrentCoordY = y1;

    if (xdistance > 1 && ydistance > 1){//complex angles
        return CheckAngledRange(From,To);
    }
    else if (xdistance == 1 && ydistance > 1){//vertical + 1 horisoltal
        while (CurrentCoordY - ydirection != y2 && BlankSpot(CurrentCoordX, CurrentCoordY - ydirection)){//checks emptiness
            CurrentCoordY = CurrentCoordY - ydirection;
        }
        if (BlankSpot(CurrentCoordX - xdirection, CurrentCoordY - ydirection)
            || CurrentCoordX - xdirection == x2 && CurrentCoordY - ydirection == y2){
            CurrentCoordX = CurrentCoordX - xdirection;
            CurrentCoordY = CurrentCoordY - ydirection;
            while (CurrentCoordY != y2 && (BlankSpot(CurrentCoordX, CurrentCoordY - ydirection) ||
                CurrentCoordX == x2 && CurrentCoordY - ydirection == y2)){//checks emptiness
                CurrentCoordY = CurrentCoordY - ydirection;
            }
        }
    }
    else if (ydistance == 1 && xdistance > 1){// horisontal + 1 vertical
        while (CurrentCoordX - xdirection != x2 && BlankSpot(CurrentCoordX - xdirection, CurrentCoordY)){//checks emptiness
            CurrentCoordX = CurrentCoordX - xdirection;
        }
        if (BlankSpot(CurrentCoordX - xdirection, CurrentCoordY - ydirection)
            || CurrentCoordX - xdirection == x2 && CurrentCoordY - ydirection == y2){
            CurrentCoordX = CurrentCoordX - xdirection;
            CurrentCoordY = CurrentCoordY - ydirection;
            while (CurrentCoordX != x2 && (BlankSpot(CurrentCoordX - xdirection, CurrentCoordY) ||
                CurrentCoordX - xdirection == x2 && CurrentCoordY == y2)){//checks emptiness
                CurrentCoordX = CurrentCoordX - xdirection;
            }
        }
    }
    else if (xdistance == 1 && ydistance == 1){// 1 diagonal
        CurrentCoordX = CurrentCoordX - xdirection;
        CurrentCoordY = CurrentCoordY - ydirection;
    }
    else if (xdistance == 0 && ydistance > 0){//vertical
        while (CurrentCoordY != y2 && (BlankSpot(CurrentCoordX, CurrentCoordY - ydirection) ||
            CurrentCoordY - ydirection == y2))
            CurrentCoordY = CurrentCoordY - ydirection;
    }
    else if (xdistance > 0 && ydistance == 0){//horisontal
        while (CurrentCoordX != x2 && (BlankSpot(CurrentCoordX - xdirection, CurrentCoordY) ||
            CurrentCoordX - xdirection == x2))
            CurrentCoordX = CurrentCoordX - xdirection;
    }

    if (CurrentCoordX == x2 && CurrentCoordY == y2){
        return true;
    }
    else {
        return false;
    }
}
bool Player::GetFastPath(vector<Point> & OutPath,Point Start, Point End, int MaxRange){
    if (MaxRange <= 0 || !BlankPoint(Start))
        return false;
    if (Start == End){
        OutPath.push_back(Start);
        return true;
    }
    Point Direction = CreatePoint(sign(End.X-Start.X),sign(End.Y-Start.Y));

    if (!GetFastPath(OutPath, Start + Direction, End, MaxRange - 1)){
        if (abs(Direction) == CreatePoint(1, 1)){
            if (!GetFastPath(OutPath, Start + CreatePoint(Direction.X, 0), End, MaxRange - 1) &&
                !GetFastPath(OutPath, Start + CreatePoint(0, Direction.Y), End, MaxRange - 1))
                return false;
        }
        else{
            //right here, abs(Direction) is (1,0) or (0,1), we want the opposite one of what it is
            Point AddPoint = Direction.X == 0 ? CreatePoint(1, 0) : CreatePoint(0, 1);
            if (!GetFastPath(OutPath, Start + AddPoint, End, MaxRange - 1) &&
                !GetFastPath(OutPath, Start - AddPoint, End, MaxRange - 1))
                return false;
        }
    }
    OutPath.push_back(Start);
    return true;
}
bool Player::GetPath(vector<Point> & OutPath,Point Start, Point End, int MaxRange){
    OutPath.clear();
    if (Start == End){
        OutPath.push_back(Start);
        return true;
    }
    Array2d<bool> HasPassed(false);
    struct Path{
        Point P;
        Path * LastPath;
        Path(){ LastPath = NULL; }
        Path(Point InP, Path * InLastPath){
            P = InP;
            LastPath = InLastPath;
        };
        void PushPath(vector<Point> & OutPath){
            if (LastPath != NULL)
                LastPath->PushPath(OutPath);
            OutPath.push_back(P);
        }
    };
    vector<vector<Path>> Paths(MaxRange);
    //sets the original point in the vector
    Paths[0].push_back(Path(Start, NULL));
    for (int r = 0; r < MaxRange - 1; r++){
        vector<Path> & PrevPath = Paths[r];
        vector<Path> & CurPath = Paths[r+1];
        for (Path & PPath : PrevPath){
            for (Point NewP:SquareIterate(PPath.P, 1)){
                //not necessary, but possibly a more efficient alternative to the bool array method
                //bool IsFurthurFromDest = GetBoardDistance(NewP, Start) >= GetBoardDistance(PPath.P, Start);
                //bool IsCloserToEnd = GetBoardDistance(NewP, End) <= GetBoardDistance(PPath.P, End);
                if (!HasPassed[NewP] && BlankPoint(NewP)){// && (IsFurthurFromDest || IsCloserToEnd)){
                    HasPassed[NewP] = true;
                    CurPath.push_back(Path(NewP, &PPath));
                    if (NewP == End){
                        Path NewPath = Path(NewP, &PPath);
                        NewPath.PushPath(OutPath);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
bool Player::CheckMovement(Point Start, Point End, int MovePoints){
    if (GetBoardDistance(Start, End) > MovePoints)
        return false;
    vector<Point> CheckPath;
    int SaveOccType = PlayerOcc[Start];
    PlayerOcc[Start] = -1;
    bool FastVal;
    try { FastVal = GetFastPath(CheckPath, Start, End, MovePoints + 1); } catch (...){}//cannot allow throws because it would upset PlayerOcc
    PlayerOcc[Start] = SaveOccType;
    if (FastVal){
        return true;
    }
    else if (GetPath(CheckPath, Start, End, MovePoints + 1)){
        return true;
    }
    else
        return false;
}
int Player::GetPointsForMove(Point Start, Point End,int MaxMovePoints){
    vector<Point> CheckPath;
    if (GetPath(CheckPath, Start, End, MaxMovePoints + 1)){
        //checkpath counts both the beginning and the end of the path, so minus one
        int MoveLength = CheckPath.size() - 1;
        if (MoveLength <= MaxMovePoints)
            return MoveLength;
        else
            throw "Path length exceeds MaxMovePoint";//should not ever happen
    }
    else
        throw "Path failed in GetPointsForMove";//should not ever happen
}
