#include "stdafx.h"
#include "Group.h"

int GetDistance(Group * G1,Group * G2){
    return GetBoardDistance(G1->GetCen(), G2->GetCen());
}
bool AreSameType(Group * G1,Group * G2){
    return G1->Type == G2->Type;
}
GroupTypes::GroupType GetBestType(GroupTypes::GroupVal & Groups){
    using namespace GroupTypes;

    map<double, GroupType> Ordered;
    Ordered[Groups.BurnVal] = Burn;
    Ordered[Groups.DefendVal] = Defend;
    Ordered[Groups.FullAttackVal] = FullAttack;
    Ordered[Groups.RaidVal] = Raid;

    map<double, GroupType>::reverse_iterator MaxPlace = Ordered.rbegin();
    return MaxPlace->second;//returns the thing the maximum key points to
}
Group::Group(GroupTypes::GroupType MakeType){
    using namespace GroupTypes;
    using namespace Attach;
    Type = MakeType;
    switch(MakeType){
    case FullAttack:
        PreferedSize = 15;

        Attachments[BA] = 5;
        Attachments[Armor] = 3;
        Attachments[CataHorse] = 2;
        break;
    case Defend:
        PreferedSize = 10;//doesnt really matter

        Attachments[BA] = 10;
        break;
    case Raid:
        PreferedSize = 5;

        Attachments[BA] = 3;
        Attachments[Horse] = 2;
        Attachments[CataHorse] = 2;
        break;
    case Burn:
        PreferedSize = 5;

        Attachments[BA] = 3;
        Attachments[Torch] = 2;
        break;
    };
}
Group::~Group(void){}
int Group::GetMovement(){//gets minimum movement
    int MinMove = 1000;
    for (Troop * T : Troops){
        MinMove = min(T->MovementPoints,MinMove);
    }
    return MinMove;
}
int Group::GetRange(){//gets average range, rounded down
    double AvRange = 0;
    for(Troop * T:Troops){
        AvRange += T->Range;
    }
    return AvRange / Troops.size();
}
int Group::GetValue(){
    int Value = 0;
    for(Troop * T:Troops){
        Value += T->GetValue();
    }
    return Value;
}
Point Group::GetCen(){
    Point Cen = { 0, 0 };
    for (Troop * T : Troops){
        Cen += T->Spot;
    }
    Cen /= Troops.size();
    return Cen;
}
int Group::GetWidth(){
    //basically makes a rectangle of the width over 2 = height
    int Size = Troops.size();
    int Height = max(1.0,sqrt(Size)/2);
    int Width = Size / Height;
    return Width;
}
int Group::GetAvTroopVal(){
    //Value will be great enough that we wont have to worry about rounding error
    return GetValue() / GetSize();
}
void Group::Save(ofstream & File, vector<Troop *> & AllPlayerTroops){
    File.write((char *)(&Type), sizeof(Type));

    int Size = Troops.size();
    File.write((char *)(&Size), sizeof(Size));

    for (Troop * T : Troops){
        int TNum = PointerToNum(AllPlayerTroops,T);
        File.write((char *)(&TNum), sizeof(TNum));
    }
}
void Group::Load(ifstream & File, vector<Troop *> & AllPlayerTroops){
    File.read((char *)(&Type), sizeof(Type));

    int Size;
    File.read((char *)(&Size), sizeof(Size));
    Troops.resize(Size);

    for (Troop *& T : Troops){
        int TNum;
        File.read((char *)(&TNum), sizeof(TNum));
        T = AllPlayerTroops[TNum];
    }
}
void Group::AddGroupToThis(Group * AddG){
    //adds the vectors together
    Troops.insert(Troops.end(), AddG->Troops.begin(), AddG->Troops.end());
}
double Group::GetGroupWithVal(Group * Other,int Distance){
    if (!AreSameType(this, Other))
        return 0;

    //puts a severe penalty if the group movement of either is diminished
    int ThisMove = this->GetMovement();
    int OtherMove = Other->GetMovement();
    int CombinedMovement = min(ThisMove, OtherMove);
    //makes the movement change a liniar dependancy of value, might want to tweak
    double MoveDiminishVal = ((double(ThisMove) / CombinedMovement) - 1) * this->GetValue();//ratio is equal to 1 if they the values are equal
    MoveDiminishVal += ((double(OtherMove) / CombinedMovement) - 1) * Other->GetValue();

    int Dis = ceil(double(Distance) / this->GetMovement());//rounds up the division

    //val is a penalty only grouping mechanism
    double Val = -Square(Dis);

    int AdjSize = PreferedSize - GetSize();
    Val += Square(AdjSize) * sign(AdjSize);

    return Val;
}
//bool GroupIsEquiped(Group * G){
//	double Val;
//	Attach::Attachment BuildAttach;
//}
ValInfo<Attach::Attachment> Group::GetAttachAndVal(Troop * T){
    using namespace Attach;
    ValInfo<Attachment> BestAttach(0,NoAttach);

    for (AttachVals::iterator Iter = Attachments.begin(); Iter != Attachments.end(); Iter++){
        ValInfo<Attachment> ThisAttach(Iter->first, Iter->second);
        if (!T->HasAttach(ThisAttach.Info) && T->CanHaveAttach(ThisAttach.Info))
            return ThisAttach;
    }
    return ValInfo<Attachment> (0,NoAttach);
}
