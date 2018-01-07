#include "stdafx.h"
#include "ComputerPlayer.h"
const int FutureMovesAhead = 5;
Modifier GetModifier(Group * G){
    using namespace GroupTypes;
    GroupTypes::GroupType GType = G->Type;
    //these compensate for imcomplete representations of the group types
    //eventually, this should go away
    Modifier Mod{ 0, 0, 0, 0 };
    Mod.ThreatMod = 1.0 / G->GetSize();
    Mod.AttackMod = 1.0 / G->GetSize();
    Mod.DefendMod = 1.0;
    Mod.LandMod = 1.0;
    switch (G->Type){
    case FullAttack:
        Mod.DefendMod /= 3;
        break;
    case Defend:
        Mod.DefendMod *= 2;
        Mod.LandMod /= 2;
        break;
    case Raid:
        Mod.LandMod *= 2;
        break;
    case Burn:
        break;
    }
    return Mod;
}
SimpleCompPlayer::SimpleCompPlayer(void):EPlayer(){
    Type = Computer;
}
SimpleCompPlayer::SimpleCompPlayer(Player * Play) : EPlayer(Play){
    Type = Computer;
}
SimpleCompPlayer::~SimpleCompPlayer(void){
    ClearData();
}
ComputerPlayer::ComputerPlayer(void) :SimpleCompPlayer(){};
ComputerPlayer::~ComputerPlayer(void){};
EPlayer::EPlayer(Player * Play) :Player(Play){
    DeepCopy(Play);
};
void EPlayer::DeepCopy(Player * CopyFrom){
    Player::DeepCopy(CopyFrom);
    CalcPowerAndCenter();
}
void EPlayer::CalcPowerAndCenter(){
    //should already have info associated with the player before this is called
    MilitPower = 0;
    Center = { 0, 0 };
    for (Troop * T : Troops){
        double ThisPower = T->GetValue() * ValOfTroopOverCash;//little bit of a boost for being active troops instead of buildings
        MilitPower += ThisPower;

        Center += T->Spot * ThisPower;
    }
    TotalPower = MilitPower;

    for (Building * B : Buildings){
        double ThisPower = B->GetCost();
        TotalPower += ThisPower;

        Center += B->GetAvPoint() * ThisPower;
    }
    //at this point the center numbers are basicaly the power of each square multiplied by the power of each square, so this works
    Center /= TotalPower;

    TotalPower += Money;
}
void SimpleCompPlayer::ClearData(){
    Player::ClearData();

    for (EPlayer * Play : AllPlayers)
        if (Play != this)
            delete Play;

    AllPlayers.clear();
    Enemies.clear();
    Friendlies.clear();

    for(Group * G:Groups)
        delete G;

    Groups.clear();
}
void SimpleCompPlayer::Draw(){
    Player::Draw();
#ifdef Debug_Macro_Move
    int * AcLevels = Levels + 1;
    switch (Levels[0]){
    case 0:
        DrawMacroMoveStuff(VecTVals, AcLevels);
        break;
    case 1:
        DrawMacroMoveBuild(VecBVals, AcLevels);
        break;
    case 2:
        DrawMicroMoveStuff(VecMVals, AcLevels);
        break;
    };
    DrawMajorChoice(NumOfMChoices, Levels[0]);
#endif
}
void SimpleCompPlayer::Save(ofstream & File){
    Player::Save(File);
    //saves the groups
    int Size = Groups.size();
    File.write((char *)(&Size), sizeof(Size));
    for (Group * G : Groups){
        GroupTypes::GroupType Type = G->Type;
        File.write((char *)(&Type), sizeof(Type));
        G->Save(File,Troops);
    }
}
void SimpleCompPlayer::Load(ifstream & File){
    Player::Load(File);
    //loads the groups
    int Size;
    File.read((char *)(&Size), sizeof(Size));
    Groups.resize(Size);
    for (Group *& G : Groups){
        GroupTypes::GroupType Type;
        File.read((char *)(&Type), sizeof(Type));

        G = new Group(Type);
        G->Load(File, Troops);
    }
}
void SimpleCompPlayer::DoAITurn(vector<Player *> & InPlayers){
    PlacePlayers(InPlayers);
    CalcPowerAndCenter();
    GetPPaths();
    GetThreat();
    InteleBuild();
    ChangeTurn();//p
    GetAntiThreat();
    GroupTroops();
    ClaimedAttackSpots.clear();
    if (Groups.size() > 0)
        MoveGroup(Groups.back());
    ChangeTurn();//p
    AttackEnemies();
    ChangeTurn();//p
    ChangeOverallTurn();
#ifdef Debug_Macro_Move
    //second and nth iterations
    GroupSquareVals gvals = GetValsOfGroupMove(Groups[0]);
    SubtractBlockingMoves(gvals, 10);
#endif
}
void SimpleCompPlayer::PlacePlayers(vector<Player *> & InPlayers){
    //sets the pointers to the players into our data
    AllPlayers.clear();
    Enemies.clear();
    Friendlies.clear();
    for (Player * Play : InPlayers){
        if (this->PlayerNum != Play->PlayerNum){
            EPlayer * NewPlay = new EPlayer(Play);//deep copy
            AllPlayers.push_back(NewPlay);
            if (OnSameTeam(NewPlay, this))//friendlies do include this class
                Friendlies.push_back(NewPlay);
            else
                Enemies.push_back(NewPlay);
        }
        else{
            Friendlies.push_back(this);
            AllPlayers.push_back(this);
        }
    }
    //simple memory optimization
    AllPlayers.shrink_to_fit();
    Enemies.shrink_to_fit();
    Friendlies.shrink_to_fit();
}
void SimpleCompPlayer::GroupTroops(){
    //for (vector<Group *>::iterator G1It = Groups.begin(); G1It != Groups.end(); G1It++){
    //	Group * G1 = *G1It;
    //	Group * BestG = NULL;
    //	double BestVal = 0;
    //	for (Group * G2 : Groups){
    //		if (G1 == G2)//if they are the same group
    //			continue;
    //
    //		ClearBoardOfGroup(G1);//this is so that CrudeGetDis works properly
    //		ClearBoardOfGroup(G2);

    //		int CombinedMovement = min(G1->GetMovement(), G2->GetMovement());

    //		int Distance = CrudeGetDis(CombinedMovement, G1->GetCen(), G2->GetCen());

    //		double Val = G2->GetGroupWithVal(G1,Distance);

    //		RestoreGroupToBoard(G1);
    //		RestoreGroupToBoard(G2);

    //		if (Val > BestVal){
    //			BestVal = Val;
    //			BestG = G2;
    //		}
    //	}
    //	if (BestG != NULL){
    //		BestG->AddGroupToThis(G1);
    //		Groups.erase(G1It);

    //		GroupTroops();//this recursion solves the iterator problem and the multiple joinings problem
    //		break;
    //	}
    //}
    if (Groups.size() <= 1)
        return;
    Group * AllGroup = Groups[0];
    for_each(Groups.begin() + 1, Groups.end(), [&](Group *& G){
        AllGroup->AddGroupToThis(G);
    });
    Groups.erase(Groups.begin() + 1, Groups.end());
}
void SimpleCompPlayer::GetPPaths(){
    EnemyPaths.clear();
    FriendPaths.clear();

    for (EPlayer * Play : Enemies)
        EnemyPaths.push_back(Vector(this->Center,Play->Center));
    for (EPlayer * Play: Friendlies)
        FriendPaths.push_back(Vector(this->Center,Play->Center));
}
void SimpleCompPlayer::GetThreat(){
    Threat.Init(SquareThreat( {}, 0, 0, 0 ));
    FutureThreat.Init(SquareThreat( {}, 0, 0, 0 ));

    for(EPlayer * Play:Enemies){
        for(Troop * T: Play->Troops){
            SquareThreat ThisThreat(T);
            for (Point AttackP : SquareIterate(T->Spot, T->GetMoveAttackRange())){
                Threat[AttackP] += ThisThreat;
            }
            int ThisMoveAttackRange = T->MovementPoints * FutureMovesAhead + T->Range;
            for (Point AttackP : SquareIterate(T->Spot, ThisMoveAttackRange)){
                FutureThreat[AttackP] += ThisThreat;
            }
        }
    }
}
void SimpleCompPlayer::GetAntiThreat(){
    AntiThreat.Init(SquareThreat( {}, 0, 0, 0 ));

    for(Troop * T: this->Troops){
        for (Point AttackP : SquareIterate(T->Spot, T->GetMoveAttackRange())){
            AntiThreat[AttackP] += SquareThreat(T);
        }
    }
}
Group * SimpleCompPlayer::FindGroup(Troop * T){
    for (Group * G : Groups){
        for (Troop * TT : G->Troops)
            if (TT == T)
                return G;
    }
    return NULL;
}
EPlayer * SimpleCompPlayer::GetPlayer(Point P){
    int POcc = PlayerOcc[P];
    return POcc >= 0 ? AllPlayers[POcc] : NULL;
}
BaseType SimpleCompPlayer::GetOccType(Point P){
    if (!BlankPoint(P))
        return GetPlayer(P)->TPlayerMap[P];
    else
        return Nothing;
}
Troop * SimpleCompPlayer::GetTroop(Point P){
    //even though it is safe, this function should only be called if one knows there is a troop there
    if (!BlankPoint(P) && GetOccType(P) == TroopType){
        Player * Play = GetPlayer(P);
        return Play->Troops[Play->NPlayerMap[P]];
    }
    else
        return NULL;
}
Building * SimpleCompPlayer::GetBuilding(Point P){
    //even though it is safe, this function should only be called if one knows there is a building there
    if (!BlankPoint(P) && GetOccType(P) == BuildingType){
        Player * Play = GetPlayer(P);
        return Play->Buildings[Play->NPlayerMap[P]];
    }
    else
        return NULL;
}

bool SimpleCompPlayer::CanMoveToSafety(Troop * T){
    bool ReturnVal = false;
    FastSquareIter(T->Spot,T->Range,[&](Point P){
        if (Threat[P].TThreat == 0){
            ReturnVal = true;
            throw 0;//break
        }
    });
    return ReturnVal;
}
int SimpleCompPlayer::CountBuildings(BuildingTypes Build){
    int Count = 0;
    for(Building * B: Buildings){
        Count += (B->Type == Build);
    }
    return Count;
}
int SimpleCompPlayer::CountTroops(TInfo::TroopTypes Type){
    int Count = 0;
    for(Troop * T:Troops){
        Count += (T->ThisType == Type);
    }
    return Count;
}
int SimpleCompPlayer::GetValOfAttack(Point P, Troop * AttackTroop){//does not do any range checking!!!!
    if (AttackTroop->AttackValue <= 0 || BlankPoint(P))//necessary to spot div by 0 error and other errors
        return 0;
    if (SimpleCompPlayer::SimulatedAttemptAttack(P, AttackTroop) == false)
        return 0;

    //now it is occupied by some sort of enemy for sure
    BaseType Type = GetOccType(P);
    if (Type == TroopType){
        Troop * DefendTroop = GetTroop(P);
        if (DefendTroop->AttemptAttack(AttackTroop)){
            double HPTaken = min(AttackTroop->AttackValue, DefendTroop->Hitpoints);
            return (DefendTroop->GetValue() * HPTaken) / DefendTroop->Hitpoints;
        }
        else
            return 0;
    }
    else if (Type == BuildingType){
        Building * DefendBuild = GetBuilding(P);
        if (DefendBuild->AttemptAttack(AttackTroop))
            return DefendBuild->GetCost() / DefendBuild->GetNumOfSquares();
        else
            return 0;
    }
    else
        return 0;
}
void SimpleCompPlayer::ClearBoardOfGroup(Group * G){//removes group from the PlayerOcc so it can think through its moves better
    for (Troop * T : G->Troops)
        PlayerOcc[T->Spot] = -1;
}
void SimpleCompPlayer::RestoreGroupToBoard(Group * G){//places troops on board if they were removed
    for (Troop * T : G->Troops)
        PlayerOcc[T->Spot] = PlayerNum;
}

int SimpleCompPlayer::CrudeGetDis(int Movement,Point StartP,Point EndP){
    const int StartMovePoints = 15;//increase for logicial security, decrease for performance
    int RealDistance;
    if (CheckMovement(StartP, EndP, StartMovePoints))
        RealDistance = GetPointsForMove(StartP, EndP, StartMovePoints);
    else
        return max(GetBoardDistance(StartP, EndP) / Movement,const_cast<int &>(StartMovePoints)) + 1;//assumes that it is over movepoints

    return RealDistance;
};
const static double StdDisVal = 5;
const static double AttackDisVal = 50;
PointVal SimpleCompPlayer::GetBestAttackPoint(Point NewTroopP,Troop * AttackTroop){
    PointVal KillVal(0,CreatePoint(0,0));
    for (Point TP:SquareIterate(NewTroopP, AttackTroop->Range)){
        if (!BlankPoint(TP) && GetPlayer(TP)->TeamNum != this->TeamNum){//if there is an enemy thing there
            if (CheckAttack(NewTroopP, TP, AttackTroop->Range)){//if it is within range
                PointVal ThisVal(GetValOfAttack(TP, AttackTroop), TP);
                if (ThisVal.Val > KillVal.Val)
                    KillVal = ThisVal;
            }
        }
    }
    return KillVal;
}

bool SimpleCompPlayer::SimulatedAttemptAttack(Point Loc,Troop * AttackingTroop){
    Player * Play = GetPlayer(Loc);

    if (Play != NULL)
        return Play->AttemptAttackOnSelf(Loc, AttackingTroop);
    else
        return false;
}
void SimpleCompPlayer::SimulatedAttackLocation(Point Loc, Troop * AttackingTroop){
    Player * Play = GetPlayer(Loc);

    if (Play != NULL
        && !OnSameTeam(Play,this)
        && Play->AttemptAttackOnSelf(Loc, AttackingTroop))
            Play->PlayerAttackLocation(Loc, AttackingTroop);
}
void SimpleCompPlayer::AttackEnemies(){
    //Will not attack a square if something blocked the claim spot!!!!
    set<Troop *> FailedAttacks;
    for (auto & AttackPair:ClaimedAttackSpots){
        Troop * T = AttackPair.first;
        Point P1 = T->Spot,P2 = AttackPair.second;
        if (BlankPoint(P2) || !CheckAttack(P1, P2, T->Range)){
            FailedAttacks.insert(T);//this is a logical error, but I will take care of it anyways
            continue;
        }
        ::AttackLocation(P2, T);
        SimulatedAttackLocation(P2, T);
    }
    for (Troop * T : FailedAttacks){
        PointVal BestP = GetBestAttackPoint(T->Spot, T);
        //all checks have been passed in GetBestAttackPoint
        if (BestP.Val > 0){
            ::AttackLocation(BestP.Info, T);
            SimulatedAttackLocation(BestP.Info, T);
        }
    }
}
