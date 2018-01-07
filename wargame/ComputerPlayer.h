
#include "Player.h"
#include "Group.h"
struct AttackInfo{
    Point MoveP;
    Point AttackP;
    Troop * T;
};
struct Modifier{
    double ThreatMod;
    double AttackMod;
    double DefendMod;
    double LandMod;
};
#ifndef Debug_Macro_Move
typedef RangeArray<double>  RangeValArray;
typedef RangeValArray AttackSquareVals;
struct MoveSquareVals{
    AttackSquareVals AttackV;
    double MoveV;
};
typedef PartialRangeArray<MoveSquareVals>  MoveSquareVals;
typedef map<Troop *, MoveSquareVals> GroupSquareVals;
template<typename Ty>
struct TroopInfo{
    Troop * T;
    int TeamNum;
    int PlayerNum;
    Ty Info;
};
struct TroopInf{
    Troop * T;
    int TeamNum;
    int PlayerNum;
};
#endif
template<typename Ty>
struct BuildInfo{
    Building * B;
    int TeamNum;
    Ty Info;
};
struct SquareThreat{
    set<Troop *> ThreatTroops;
    int MaxAttack;
    int BThreat;
    int TThreat;
    SquareThreat(Troop * ThreatT){
        if (ThreatT->TAttackType == TroopAndBuild)
            BThreat = 1;
        else
            BThreat = 0;
        TThreat = 1;
        MaxAttack = ThreatT->AttackValue;
        ThreatTroops.insert(ThreatT);
    }
    SquareThreat(set<Troop *> InThreatTroops, int InMaxAttack, int InBThreat, int InTThreat){
        ThreatTroops = InThreatTroops;
        MaxAttack = InMaxAttack;
        BThreat = InBThreat;
        TThreat = InTThreat;
    }
    SquareThreat() = default;
    void operator += (const SquareThreat & Other){
        MaxAttack = max(Other.MaxAttack,MaxAttack);
        BThreat += Other.BThreat;
        TThreat += Other.TThreat;
        InsertSet(ThreatTroops, Other.ThreatTroops);
    }
};
Modifier GetModifier(Group * G);
int GetAdjRange(int MoveNum, int MoveRange);
double TroopBuyRatios(TInfo::TroopTypes TType);
int DistanceToVector(Point P, Vector V);
bool BuildIsMilitary(BuildingTypes Build);
bool BuildIsCivilian(BuildingTypes Build);
double GetAvBuildIncome(Player * Play);
double GetAvValueOfTroops(Player * Play);
double GetValOfDom(Player * Play);
const double ValOfTroopOverCash = 1.25;
class EPlayer :
    public Player
{
public:
    virtual ~EPlayer(void){};
    EPlayer(Player * Play);
    EPlayer(void) :Player(){};
    virtual void DeepCopy(Player * CopyFrom);
    void CalcPowerAndCenter();
    double TotalPower;//basically the cost of the players stuff
    double MilitPower;//cost of the troops
    Point Center;
};
class SimpleCompPlayer :
    public EPlayer
{
public:
    SimpleCompPlayer(void);
    virtual ~SimpleCompPlayer(void);
    SimpleCompPlayer(Player * Play);
#ifdef Debug_Macro_Move
    vector<DArray2d<TroopInfo<Array2d<double>>>> VecTVals[4];
    DArray2d<Array2d<double>> VecBVals;
    vector<vector<MoveSquareVals>> VecMVals;
#endif
    Array2d<SquareThreat> Threat;//the threat enemy groups have on this group
    Array2d<SquareThreat> FutureThreat;//Futurethreat stacks up threat for FutureMovesAhead and ignores bariers
    Array2d<SquareThreat> AntiThreat;//the threat this groups troops have on enemy groups
    vector<int> TroopGroup;// this is a representation of what group each troop is in
    vector<Group *> Groups;
    vector<EPlayer *> AllPlayers;//all of the players are deep copied into here by placeplayers
    vector<EPlayer *> Enemies;
    vector<EPlayer *> Friendlies;//this includes this class!
    vector<Vector> EnemyPaths;
    vector<Vector> FriendPaths;
    BuildList RanOut;
    BuildList SoughtAfter;
    map<Troop *,Point> ClaimedAttackSpots;
    double EnemyOverFriendRatio;
    GroupTypes::GroupVal TotGroupValue;
    virtual void DoAITurn(vector<Player *> & InPlayers);
    Group * FindGroup(Troop * T);
    void PlacePlayers(vector<Player *> & InPlayers);
    void ClearData();
    void Draw();
    virtual void Save(ofstream & File);
    virtual void Load(ifstream & File);
    void PlaceNewTroop(Point spot, TInfo::TroopTypes ThingType);
    virtual void RemoveThingCompletely(BaseType ThingType,int Item);
    void GroupTroops();
    void GetThreat();
    void GetAntiThreat();
    BaseType GetOccType(Point P);
    Troop * GetTroop(Point P);
    Building * GetBuilding(Point P);
    EPlayer * GetPlayer(Point P);
    int EnemyMilitaryStrength;
    template<typename Fnc>
    void IterOverMoveAttack(Troop * T, Fnc FIter);//Fnc takes a Point as its single argument
    template<typename Fnc>
    void IterOverMove(Troop * T, Fnc FIter);//Fnc takes a Point as its single argument
    template<typename Fnc>
    void IterOverAttack(Troop * T,Point MoveP, Fnc FIter);//Fnc takes a Point as its single argument
    template<typename Fnc>
    void IterThroughTValBlockedAttacks(Point MoveP,Troop * T,GroupSquareVals & IterVals, Fnc Fn);
    template<typename Fnc>
    void IterThroughTValBlockedMoves(Point MoveP, GroupSquareVals & IterVals, Fnc Fn);
    template<typename Fnc>
    void IterThroughAttackSquaresOfTroops(Point AttackP, Troop * ThisT, GroupSquareVals & IterVals, Fnc Fn);
    template<typename Fnc>
    void IterThroughBlockingMoves(AttackInfo MoveInfo, GroupSquareVals & IterVals, Fnc Fn);
    void GetPPaths();
    void GetPPower();
    void InteleBuild();
    void ClearBoardOfGroup(Group * G);
    void RestoreGroupToBoard(Group * G);
    double GetDefaultBuyValue(Troop * T,const Array2d<double> & Values);
    Array2d<double> GetDefaultTSqVals(Troop * T);
    double GetValOfTroopWithAttach(Troop * T, Attach::Attachment Attc);
    ValInfo<pair<Troop *,Attach::Attachment>> FindBestSolierAttachValueAndNum();
    ValInfo<BuildingTypes> FindBestBuildType(bool TroopCountNotMet);
    pair<Point, Point> FindBestBuildSpot(BuildingTypes Build);
    TInfo::TroopVals FindTroopValues();
    ValInfo<TInfo::TroopTypes> FindBestTroopValue();
    triple<double,Point, Building *> FindBestTroopSpot(TInfo::TroopTypes TType);

    bool CanMoveToSafety(Troop * T);
    int CountBuildings(BuildingTypes Build);
    int CountTroops(TInfo::TroopTypes Type);
    int GetValOfAttack(Point P, Troop * AttackTroop);
    void DetermineGroupActions();

    PartialRangeArray<ValInfo<vector<Point>>> GetPaths(Point Start, int MoveRange, Array2d<double> Vals);
    vector<TroopInfo<PartialRangeArray<ValInfo<vector<Point>>>>> GetInteractingPaths();
    int CrudeGetDis(int Movement, Point StartP, Point EndP);//only works well if there is a clear path!!!!!!
    Array2d<double> GetValuesOfTroopSquares(Troop * ThisT,Attach::AttachVals TroopAttachVals,Modifier Mod);
    ValInfo<Point> GetBestAttackPoint(Point NewTroopP,Troop * AttackTroop);
    void RemoveMoveableTroops();

    void SubtractBlockingMoves(GroupSquareVals & TVals, int NumOfIters);
    void SubtractBlockingMove(GroupSquareVals & TVals, GroupSquareVals & AdjVals);
    Array2d<double> GetGroupVals(Group * G);
    GroupSquareVals GetValsOfGroupMove(Group * G);
    void MoveGroup(Group * G);
    void AttackEnemies();
    bool SimulatedAttemptAttack(Point Loc, Troop * AttackingTroop);
    void SimulatedAttackLocation(Point Loc, Troop * AttackingTroop);
};
class ComputerPlayer :
    public SimpleCompPlayer
{
public:
    ComputerPlayer(void);
    virtual ~ComputerPlayer(void);


};
