#pragma once
#include "stdafx.h"
#include "TemplateStructures.h"

//debug preprocessor definitions
//#define DrawRandomPath
//#define DrawMoveValLoc
//#define ShowSquareNums
//#define ShowSubtractEffect
//#define DrawMacroPath
#define Debug_Macro_Move
typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;
class Troop;
bool IsRealPlayerTurn();
#define AttachBuyRange 5
#include "PointOperators.h"
typedef ValInfo<Point> PointVal;
#ifdef Debug_Macro_Move
typedef RangeArray<double>  RangeValArray;
typedef RangeValArray AttackSquareVals;
struct MoveSquareVal{
    AttackSquareVals AttackV;
    double MoveV;
};
typedef PartialRangeArray<MoveSquareVal>  MoveSquareVals;
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

#ifdef Debug_Macro_Move
#define MaxLevels 5
#define NumOfMChoices 3
extern int Levels[MaxLevels];
#endif
namespace Attach{
    //make sure that these values go from 0 to NumOfAttachments
    enum Attachment{ NoAttach = 0, Torch = 1, BA = 2, Horse = 3, Armor = 4, CataHorse = 5};//change GlobVars!
#define NumOfAttachments 6
    typedef ThingList<Attachment> AttachList;
    extern AttachList FullAttachList;
    typedef ThingVals<Attachment, NumOfAttachments> AttachVals;
    inline int GetAttachCost(Attachment Attc){
        switch (Attc){
        case Horse:return 50;
        case Torch:return 20;
        case BA:return 20;
        case Armor:return 30;
        case CataHorse:return 100;
        case NoAttach:return 0;
        }
    }
}
namespace TInfo{
    //make sure that these values go from 0 to NumOfAttachments
    enum TroopTypes{NoType = -1,SoldierType = 0,CatapultType = 1,WallType = 2};//change GlobVars!
    #define NumOfTroopsTypes 3
    typedef ThingVals<TroopTypes, NumOfTroopsTypes> TroopVals;//this will be used by the computer player group system
    typedef ThingList<TroopTypes> TroopList;
    extern TroopList FullTroopList;//change this when changing TroopTypes
    inline int GetTroopCost(TroopTypes TType){
        switch (TType){
        case SoldierType:return 15;
        case CatapultType:return 100;
        case WallType:return 75;
        }
    }
    inline Attach::AttachVals GetDefaultAttachVals(TroopTypes TType){
        using namespace Attach;
        Attach::AttachVals TroopVals;
        int TroopVal = GetTroopCost(TType);
        switch (TType){
        case SoldierType:
            TroopVals[BA] = TroopVal;//says that it doubles effectiveness of troop
            TroopVals[Armor] = TroopVal / 2;
            TroopVals[Horse] = TroopVal / 4;
            TroopVals[Torch] = TroopVal / 8;
            break;
        case CatapultType:
            TroopVals[CataHorse] = TroopVal/3;//says that it increases effectiveness of troop by 33%
            break;
        }
        return TroopVals;
    }
    inline Attach::AttachList GetAvaliableAtts(TroopTypes TType){
        using namespace Attach;
        switch (TType){
        case SoldierType:
            return{ Torch, BA, Horse, Armor };
        case CatapultType:
            return{ Horse };
        default:
            return{};
        };
    }
}
//namespace BInfo{
    #define NumOfBuildingTypes 8
    //change GlobVars! when you change this
    enum BuildingTypes {NoBuild = -1,SFarm = 0, LFarm = 1, CataFact = 2, TrainCenter = 3, BA_Factory = 4, TorchFactory = 5, Armory = 6, Stable = 7};
    typedef ThingVals<BuildingTypes, NumOfBuildingTypes> BuildVals;
    typedef ThingList<BuildingTypes> BuildList;
    extern BuildList FullBuildList;
    extern BuildList AttachBuilds;
    extern BuildList IncomeBuilds;
    extern BuildList TrainBuilds;
//};
enum SelectTypes{NoSelect = -1,SFarmSelect = 0, LFarmSelect = 1, CataFactSelect = 2, TrainCenterSelect = 3, BA_FactorySelect = 4, TorchFactorySelect = 5, ArmorySelect = 6, StableSelect = 7, WallSelect = 8};
inline BuildingTypes SelectToBuild(SelectTypes Type){
    int TypeNum = static_cast<int>(Type);
    if (TypeNum < NumOfBuildingTypes)
        return static_cast<BuildingTypes>(Type);
    else
        return NoBuild;
}
inline TInfo::TroopTypes SelectToTroop(SelectTypes Type){
    int TypeNum = static_cast<int>(Type);
    if (TypeNum >= NumOfBuildingTypes){//if it is not a building, it must be a troop
        switch (Type){
        case WallSelect:return TInfo::WallType;
        }
    }
    else
        return TInfo::NoType;
}
#define MoneyResetVal 2000
#define MaximumDomination 2000000
struct Domination{
    int32_t Player;
    int32_t Influence;
};
extern Array2d<int> PlayerOcc;
extern Array2d<Domination> PlayerDom;
#define MaxMoveDis 12
inline int RoundUpRatio(int Num, unsigned int Denom){
    return (Num + Denom - 1) / Denom;
}
template<typename NumTy>
inline NumTy Cube(NumTy Num){
    return Num * Num * Num;
}
inline bool IsBetween(double Val, double LowerBound, double UpperBound){
    return Val >= LowerBound && Val <= UpperBound;
}
template<typename Fn,typename EnumTy>
inline void IterThroughCombs(ThingList<EnumTy> & StartList,Fn Fun){
    ThingList<EnumTy> Temp;
    int size = StartList.size();
    for (int comb_iters : range(2 << size)){
        for (int item : range(size))
            if ((comb_iters >> item) & 1)
                Temp.add(static_cast<EnumTy>(item));
        Fun(Temp);
        Temp.clear();
    }
}
inline double TrueDistance(Point P1, Point P2){
    return sqrt(Square(P1.X - P2.X) + Square(P1.Y - P2.Y));
}
inline double DisConvert(int Dis){
    return 1.0 / (1 + Dis);
}
#define BlankSpot(xspot,yspot)  (PlayerOcc[xspot][yspot] < 0)
#define BlankPoint(PPoint)  (PlayerOcc[PPoint] < 0)
#define NullPoint(PPoint) (PPoint.X < 0 || PPoint.Y < 0)
inline bool OnBoard(Point P){
    return P.X < BoardSizeX && P.Y < BoardSizeY && P.X >= 0 && P.Y >= 0;
}
template<typename ThisSet>
inline void InsertSet(ThisSet & Into,const ThisSet & From){
    Into.insert(From.begin(),From.end());
}
inline int GetBoardDistance(Point P1,Point P2){
    return max(abs(P1.X - P2.X),abs(P1.Y - P2.Y));
}
inline int GetBoardDistance(int x1,int y1,int x2,int y2){
    return max(abs(x1 - x2), abs(y1 - y2));
}
template<typename Ty>
Ty max(const vector<Ty> & Vec){
    auto MaxVal = Vec.front();
    for (auto & Num : Vec)
        if (MaxVal < Num)
            MaxVal = Num;
    return MaxVal;
}
template<typename Ty>
Ty min(const vector<Ty> & Vec){
    auto MinVal = Vec.front();
    for (auto & Num : Vec)
        if (MinVal > Num)
            MinVal = Num;
    return MinVal;
}
template <typename T>
int sign(T val) {
    return (T(0) < val) - (val < T(0));
}
template<typename Ty>
Ty sum(const vector<Ty> & Vec){
    if (Vec.size() == 0)
        return Ty();
    auto Sum = Vec.front();
    for_each(Vec.begin() + 1, Vec.end(), [&](decltype(Vec.front()) Val){
        Sum += Val;
    });
    return Sum;
}
template<typename VecType, typename PointerType>
inline int PointerToNum(const VecType & V,PointerType Ptr){
    int Size = V.size();
    for (int g = 0; g < Size; g++){
        if (V[g] == Ptr)
            return g;
    }
}
template<typename MapType>//the map must have the value that needs to maximized as the store type
inline auto MaxOf(const MapType & ThisMap) -> decltype(ThisMap.begin()){
    using itty = decltype(ThisMap.begin());
    itty MaxIt = ThisMap.begin();
    for (itty It = ThisMap.begin(); It != ThisMap.end(); It++){
        if (It->second > MaxIt->second){
            MaxIt = It;
        }
    }
    return MaxIt;
}
template<typename FuncType>
void FastSquareIter(Point CenP,int Range,FuncType Func){
    try{
        Point OutP;
        int MaxX = min(CenP.X + Range, BoardSizeX - 1);
        int MinX = max(CenP.X - Range, 0);
        int MaxY = min(CenP.Y + Range, BoardSizeY - 1);
        int MinY = max(CenP.Y - Range, 0);

        for (OutP.X = MinX;
            OutP.X <= MaxX;
            OutP.X++){
            for (OutP.Y = MinY;
                OutP.Y <= MaxY;
                OutP.Y++){
                Func(OutP);
            }
        }
    }
    catch (...){}
}
template<typename FuncType>
void FastCoordSquareIter(Point CenP, int Range, FuncType Func){
    try{
        for (int X = max(CenP.X - Range, 0);
            X <= min(CenP.X + Range, BoardSizeX - 1);
            X++){
            for (int Y = max(CenP.Y - Range, 0);
                Y <= min(CenP.Y + Range, BoardSizeY - 1);
                Y++){
                Func(X,Y);
            }
        }
    }
    catch (...){}
}
template<typename FuncType>
void FastExpandingIter(Point CenP,int StartRange,int MaxRange, FuncType Express){
    Point OutP;
    try{
        if (StartRange == 0 && MaxRange > 0)//special case of zero needs this attention
            Express(CenP);
        for (int Range = StartRange; Range < MaxRange; Range++){
            OutP.Y = CenP.Y - Range;
            if (OutP.Y >= 0){
                for (OutP.X = max(CenP.X - Range, 0); OutP.X < min(CenP.X + Range, BoardSizeX - 1); OutP.X++){
                    Express(OutP);
                }
            }
            OutP.Y = CenP.Y + Range;
            if (OutP.Y < BoardSizeY){
                for (OutP.X = min(CenP.X + Range, BoardSizeX - 1); OutP.X > max(CenP.X - Range, 0); OutP.X--){
                    Express(OutP);
                }
            }
            OutP.X = CenP.X - Range;
            if (OutP.X >= 0){
                for (OutP.Y = max(CenP.Y - Range, 0); OutP.Y < min(CenP.Y + Range, BoardSizeY - 1); OutP.Y++){
                    Express(OutP);
                }
            }
            OutP.X = CenP.X + Range;
            if (OutP.X < BoardSizeX){
                for (OutP.Y = min(CenP.Y + Range, BoardSizeY - 1); OutP.Y > max(CenP.Y - Range, 0); OutP.Y--){
                    Express(OutP);
                }
            }
        }
    }
    catch (...){}
}
template<typename FuncType>
void FastBoardIter(FuncType Func){
    Point P;
    try{
        for (P.X = 0; P.X < BoardSizeX; P.X++){
            for (P.Y = 0; P.Y < BoardSizeY; P.Y++){
                Func(P);
            }
        }
    }
    catch (...){}
}
