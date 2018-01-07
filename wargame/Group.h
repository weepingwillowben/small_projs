#pragma once
#include "Player.h"
namespace GroupTypes{
    enum GroupType {Defend,FullAttack,Raid,Burn};
    enum GroupAction {GroupUp,GearUp,OffensiveMarch,DefensiveMarch};
    struct GroupVal{
        double DefendVal;
        double FullAttackVal;
        double RaidVal;
        double BurnVal;

        GroupVal(){ DefendVal = 0; FullAttackVal = 0; RaidVal = 0; BurnVal = 0; };
        GroupVal(double InDefendVal, double InFullAttackVal, double InRaidVal, double InBurnVal){
            DefendVal = InDefendVal;
            RaidVal = InRaidVal;
            FullAttackVal = InFullAttackVal;
            BurnVal = InBurnVal;
        }

        GroupVal operator * (double Val){
            //copys group over
            GroupVal New = (*this);

            New *= Val;

            return New;
        }
        void operator *= (double Val){
            DefendVal *= Val;
            FullAttackVal *= Val;
            RaidVal *= Val;
            BurnVal *= Val;
        }
        void operator += (GroupVal & AddG){
            DefendVal += AddG.DefendVal;
            FullAttackVal += AddG.FullAttackVal;
            RaidVal += AddG.RaidVal;
            BurnVal += AddG.BurnVal;
        }
        GroupVal operator + (GroupVal & AddG){
            GroupVal NewG = AddG;
            NewG += (*this);
            return NewG;
        }
        double GetTotVal(){
            return DefendVal + FullAttackVal + RaidVal + BurnVal;
        }
    };
};
GroupTypes::GroupType GetBestType(GroupTypes::GroupVal & Groups);
struct Vector{
    Point Orig;
    Point Dest;
    Vector(Point InOrig, Point InDest){
        Orig = InOrig;
        Dest = InDest;
    }
    int GetLength(){return GetBoardDistance(Orig,Dest);};
};
class Group
{
public:
    Group(GroupTypes::GroupType MakeType);
    ~Group(void);
    GroupTypes::GroupType Type;
    Attach::AttachVals Attachments;
    int PreferedSize;
    vector<Troop *> Troops;
    int GetMovement();//gets minimum movement
    int GetRange();//gets average range,rounded down
    int GetValue();
    Point GetCen();
    int GetWidth();
    int GetAvTroopVal();
    int GetSize(){ return Troops.size(); }
    void Save(ofstream & File, vector<Troop *> & AllPlayerTroops);
    void Load(ifstream & File, vector<Troop *> & AllPlayerTroops);
    void AddGroupToThis(Group * AddG);
    ValInfo<Attach::Attachment> GetAttachAndVal(Troop * T);
    double GetGroupWithVal(Group * Other,int Distance);
};
//helper functions for groups
int GetDistance(Group * G1,Group * G2);
bool AreSameType(Group * G1,Group * G2);
bool GroupIsEquiped(Group * G);
