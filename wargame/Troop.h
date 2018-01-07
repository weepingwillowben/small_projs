#pragma once
#include "Globals.h"
enum AttackType{TroopOnly,TroopAndBuild};
class Troop
{
public:
    Troop(void){};
    virtual ~Troop(void){};
    Point Spot;
    int MovementPoints;
    int Hitpoints;
    TInfo::TroopTypes ThisType;
    AttackType TAttackType;
    Attach::AttachList Attcs;
    int AttackValue;
    int Range;
    int ExploRange;
    bool HaveAttacked;
    bool HasAttach(Attach::Attachment Attc){ return Attcs.has(Attc); }
    bool CanHaveAttach(Attach::Attachment Attc){ return TInfo::GetAvaliableAtts(ThisType).has(Attc); }
    virtual void Draw() = 0;
    virtual void ResetStats() = 0;
    int GetMoveAttackRange(){return MovementPoints + Range;};
    virtual bool DestroyedAfterAttack(Troop * AttackTroop) = 0;
    virtual bool AttemptAttack(Troop * AttackTroop) = 0;
    int GetValue(){
        int Cost = TInfo::GetTroopCost(ThisType);
        for (Attach::Attachment Attc : Attcs)
            Cost += Attach::GetAttachCost(Attc);
        return Cost;
    };
    virtual void Save(ofstream & File){
        File.write((char *)(&Spot),sizeof(Spot));
        File.write((char *)(&MovementPoints),sizeof(MovementPoints));
        File.write((char *)(&ThisType),sizeof(ThisType));
        File.write((char *)(&Hitpoints),sizeof(Hitpoints));
        File.write((char *)(&TAttackType),sizeof(TAttackType));
        File.write((char *)(&AttackValue),sizeof(AttackValue));
        File.write((char *)(&Range),sizeof(Range));
        File.write((char *)(&ExploRange),sizeof(ExploRange));
        File.write((char *)(&HaveAttacked),sizeof(HaveAttacked));
        Attcs.save(File);
    }
    virtual void Load(ifstream & File){
        File.read((char *)(&Spot),sizeof(Spot));
        File.read((char *)(&MovementPoints),sizeof(MovementPoints));
        File.read((char *)(&ThisType),sizeof(ThisType));
        File.read((char *)(&Hitpoints),sizeof(Hitpoints));
        File.read((char *)(&TAttackType),sizeof(TAttackType));
        File.read((char *)(&AttackValue),sizeof(AttackValue));
        File.read((char *)(&Range),sizeof(Range));
        File.read((char *)(&ExploRange),sizeof(ExploRange));
        File.read((char *)(&HaveAttacked),sizeof(HaveAttacked));
        Attcs.load(File);
    }
};
