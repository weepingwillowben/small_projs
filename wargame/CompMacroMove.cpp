
#include "stdafx.h"
#include "ComputerPlayer.h"
struct Path{
    Point P;

    Path * LastPath;
    double CumVal;

    Path * NextPath;
    double PathVal;

    Path(){ LastPath = NULL; NextPath = NULL; CumVal = -10e50; PathVal = -10e50; }
    Path(Point InP, double AddVal, Path * InLastPath){
        P = InP;
        LastPath = InLastPath;
        CumVal = AddVal;
        NextPath = NULL;
        if (LastPath != NULL)
            CumVal += LastPath->CumVal;
    };
    void PushPath(vector<Point> & OutPath){
        OutPath.push_back(P);
        if (NextPath != NULL)
            NextPath->PushPath(OutPath);
    }
};
template<typename Ty>
TroopInfo<Ty> MakeTInfo(Troop * T,int TeamNum,int PlayerNum,Ty Info){
    return{ T,TeamNum,PlayerNum, Info };
}
template<typename Ty>
BuildInfo<Ty> MakeBInfo(Building * B, int TeamNum, int PlayerNum, Ty & Info){
    return{ B, TeamNum, PlayerNum, Info };
}
int GetAdjRange(int PlaceNum, int MoveRange){
    return PlaceNum == 0 ? 1 : (PlaceNum + MoveRange - 1) / MoveRange;
}
double GetValMoveAdjust(int MoveNum){
    return 1.0 / Square(MoveNum);
}
Array2d<double> SimpleCompPlayer::GetValuesOfTroopSquares(Troop * ThisT, Attach::AttachVals TroopAttachVals, Modifier Mod){
    Array2d<double> Vals(0);
    int Range = ThisT->Range;
    int MoveRange = ThisT->MovementPoints;

    auto AddValToRange = [&](double AddVal, Point P, int Range){
        //this function take the value that needs to be added on and spreads it over the board
        //gets the total value of the iteration do it can spread it without the total value increasing
        //double TotDiv = 0;
        //for(Point IterP : SquareIterate(P,Range)){
        //	TotDiv += 1 - GetBoardDistance(P,IterP) / double(Range);
        //}
        ////adds on the appropriate parts of the values to each part of the board
        //for(Point IterP : SquareIterate(P,Range)){
        //	int Dis = GetBoardDistance(P, IterP);
        //	double Div = (1 - Dis / double(Range)) / TotDiv;

        //	Vals[IterP] += AddVal * Div;
        //}
        for (Point P : SquareIterate(P, Range)){
            Vals[P] += AddVal;
        }
    };
    //this puts down the value of the enemy stuff on the board
    for (Point BP : BoardIterate()){
        if (!BlankPoint(BP) && GetPlayer(BP)->TeamNum != this->TeamNum)
            AddValToRange(GetValOfAttack(BP, ThisT) * Mod.AttackMod, BP, Range);
    }
    //get the value that is dependant on threat
    for (Point P : BoardIterate()){
        SquareThreat & ThisThreat = Threat[P];
        //change when attack vs defense actually becomes an issue
        double ValOfThreat = -(ThisT->GetValue() * ThisThreat.TThreat) * Mod.ThreatMod;
        Vals[P] += ValOfThreat;
    }
    //get the value that is dependant on land ownership
    map<EPlayer *, double> LandValue;
    for (EPlayer * Play : AllPlayers)
        LandValue[Play] = GetValOfDom(Play) * Mod.LandMod;

    for (Point P : BoardIterate()){
        int InfluenceToDom;
        double ThisValOfDom = LandValue[this];
        if (PlayerDom[P].Player >= 0){
            EPlayer * OccPlay = AllPlayers[PlayerDom[P].Player];

            if (OccPlay != NULL && PlayerDom[P].Influence == MaximumDomination){
                double OccValOfDom = LandValue[OccPlay];//if OccPlay == this, then the values are the same and cancel out to 0
                if (OnSameTeam(this, OccPlay))
                    ThisValOfDom -= OccValOfDom;
                else
                    ThisValOfDom += OccValOfDom;
            }
            if (OccPlay->PlayerNum == this->PlayerNum)
                InfluenceToDom = MaximumDomination - PlayerDom[P].Influence;
            else//adresses the nature of taking away enemies terretory twice as fast as making one's own
                InfluenceToDom = MaximumDomination + (PlayerDom[P].Influence / 2);
        }
        else{//if there is no one there, the player needs MaximumDomination to overcome it
            InfluenceToDom = MaximumDomination;
        }
        //if there is no need to dominate, the added value is zero and is needed to stop 0/0div
        if (InfluenceToDom == 0)
            continue;
        for (Point IterP : SquareIterate(P, Range)){
            int ThisInfluence = GetDominationOfTroop(Range, GetBoardDistance(IterP, P));
            ThisInfluence = min(ThisInfluence, InfluenceToDom);
            Vals[P] += (ThisValOfDom * ThisInfluence) / InfluenceToDom;
        }
    }
    //gets the value of defending buildings
    for (Building * B : Buildings){
        double BSpotVal = B->GetCost() / double(B->GetSize());
        for (Point BP : B->Place){
            if (FutureThreat[BP].BThreat > 0){
                //goes after the troops that could attack this building in futuremovesahead
                for (Troop * EnTroop : FutureThreat[BP].ThreatTroops){
                    if (EnTroop->TAttackType == TroopAndBuild){
                        //value is related to how far away the threatening troop is
                        int EnMovesToToBuild = GetAdjRange((GetBoardDistance(EnTroop->Spot, BP) - EnTroop->Range), EnTroop->MovementPoints); //doesn't account for blocking at all
                        //this value is currently far too large!!!
                        AddValToRange(Mod.DefendMod * BSpotVal / EnMovesToToBuild, EnTroop->Spot, Range);
                    }
                }
            }
        }
    }
    //adds in the values of the group being close to a center which it needs to buy attachments from
    for (auto AttachPair : TroopAttachVals){
        double AttachVal = AttachPair.first;
        Attach::Attachment Attc = AttachPair.second;

        if (ThisT->HasAttach(Attc) == true)//buying the attachment has no value if the troop already has it
            continue;

        if (AttachVal < 0.1)//skips the next part if negligable value
            continue;
        //this creates a map of the values where the group can buy the attachment it wants
        //warning!! it will double count same types of Attachment buildings(although this is not necessarily bad)
        for (Building * B : Buildings){
            if (B->Type == AttBuildReqs(Attc)){
                B->IterOverBuildFromDis(PlayerOcc, AttachBuyRange, [&](Point BuyP){
                    Vals[BuyP] += AttachVal;
                });
            }
        }
    }
    return Vals;
}
//LandVals is a double represenation of the influence part of PlayerDom
double GetOneSidedChanceToWin(Troop * Attack, Troop * Defend){
    if (Attack->AttackValue == 0 || !Defend->AttemptAttack(Attack))
        return 10e-50;

    int HitPerAttack = RoundUpRatio(Attack->Hitpoints, Defend->AttackValue);

    int RangeDif = Attack->Range - Defend->Range;

    int MoveDif = Attack->MovementPoints - Defend->MovementPoints;

    //if this is the case, the defending troop can simply attack and retreat over and over again, and so is in a very solid position
    if (RangeDif <  MoveDif - Defend->MovementPoints)
        return 10e-50;

    int MovesToCloseRange =
        (RangeDif > MoveDif);

    return HitPerAttack * (1 + 2 * MovesToCloseRange);
}
double GetChanceToWin(Troop * Attack, Troop * Defend){
    double AttackWin = GetOneSidedChanceToWin(Attack, Defend);
    double DefWin = GetOneSidedChanceToWin(Defend, Attack);
    return AttackWin / (AttackWin + DefWin);
}
void AffectTroopVals(Array2d<double> & TVals, Troop * Attack,Troop * Defend){
    //here TVals is associated with Attack

}
Array2d<uint16_t> FindMoveDistances(Point Start, int MoveRange, int StopDis){
    //This section makes an Array2d that represents the Board in terms of the minimum number of moves it takes to get to each square
    Array2d<uint16_t> ShortMoves(uint16_t(0));

    vector<Point> CurSpots;
    vector<Point> PrevSpots;
    PrevSpots.push_back(Start);
    int MoveTurn = 1;
    ShortMoves[Start] = MoveTurn;
    while (PrevSpots.size() > 0 && MoveTurn <= StopDis + 1){
        CurSpots.clear();
        for (Point PrevP : PrevSpots){
            for (Point CheckP : SquareIterate(PrevP, 1)){
                if (BlankPoint(CheckP) && ShortMoves[CheckP] == 0){
                    ShortMoves[PrevP] = MoveTurn;
                    CurSpots.push_back(CheckP);
                }
            }
        }
        MoveTurn++;
        PrevSpots = CurSpots;
    }
    //converts it from squares away to moves away
    for (uint16_t & Moves : ShortMoves){
        Moves = (Moves + MoveRange - 1) / MoveRange;
    }
    return ShortMoves;
}
void AttachPrevPaths(Array2d<Path> & PrevPaths, Array2d<Path> & CurPaths, double ValMoveAdjust,Array2d<double> & Vals){
    for (Point CurP : BoardIterate()){
        if (!BlankPoint(CurP))
            continue;
        Path * BestPrevPath = &PrevPaths[CurP];//sets an arbitrary starting point
        for (Point PrevP : SquareIterate(CurP, 1)){
            if (PrevPaths[PrevP].CumVal > BestPrevPath->CumVal)
                BestPrevPath = &PrevPaths[PrevP];
        }
        if (BestPrevPath->CumVal > -1000000.0)//if there is an active square that works
            CurPaths[CurP] = Path(CurP, Vals[CurP] * ValMoveAdjust, BestPrevPath);//CurPath is not yet initialized
    }
}
void AttachCurPaths(Array2d<Path> & CurPaths, Array2d<Path> & NextPaths,Point Start){
    for (Point CurP : BoardIterate()){
        if (!BlankPoint(CurP))
            continue;
        Path * BestNextPath = &NextPaths[CurP];//sets an arbitrary starting point
        for (Point NextP : SquareIterate(CurP, 1)){
            if (NextPaths[NextP].PathVal > BestNextPath->PathVal)
                BestNextPath = &NextPaths[NextP];
        }
        if (BestNextPath->LastPath != NULL || BestNextPath->P == Start){//if there is an active square that works
            Path & CurPath = CurPaths[CurP];
            CurPath.NextPath = BestNextPath;
            //Since the PathVal = the best ones added up, this takes the PathVal - BestOne + ThisOne to find the value of this path
            CurPath.PathVal = BestNextPath->PathVal - (BestNextPath->LastPath->CumVal - CurPath.CumVal);
        }
    }
}
PartialRangeArray<ValInfo<vector<Point>>> ConvertToMovePaths(vector<Array2d<Path>> & Paths, Point Start, int MoveRange){
    //returns the useful values in a more sensible container, a blocked out 2d array around the MoveRange
    PartialRangeArray<ValInfo<vector<Point>>> PathVals(Start, MoveRange);

    Array2d<uint16_t> PathNums = FindMoveDistances(Start, MoveRange, MoveRange);

    int MoveNum = MoveRange;
    Array2d<Path> & MovePaths = Paths[MoveNum];
    for (Point MoveP : BoardIterate()){
        if (PathNums[MoveP] <= 0)
            continue;

        PathVals.SetExist(MoveP, true);

        ValInfo<vector<Point>> & ThisVal = PathVals[MoveP];
        ThisVal.Val = MovePaths[MoveP].PathVal;
        MovePaths[MoveP].PushPath(ThisVal.Info);//ThisVal = the path
    }
    return PathVals;
}
void SimpleCompPlayer::RemoveMoveableTroops(){
    for (EPlayer * Play : AllPlayers)
        for (Troop * T : Play->Troops)
            if (T->MovementPoints > 0)
                PlayerOcc[T->Spot] = -1;
}
PartialRangeArray<ValInfo<vector<Point>>> SimpleCompPlayer::GetPaths(Point Start, int MoveRange,Array2d<double> Vals){

    const int MaxRange = max(BoardSizeX, BoardSizeY);
    const int MaxMoves = RoundUpRatio(MaxRange, MoveRange);
    if (MoveRange == 0){
        throw "Getpaths doesn't work for range 0 troops!";
    }

    auto Protectkey = Protect(PlayerOcc);
    RemoveMoveableTroops();

    if (!BlankPoint(Start))
        throw "Start needs to be empty for GetPaths to work properly";

    vector<Array2d<Path>> Paths(MaxRange);//the paths are to their neighbors that led to them

    /*
    This creates each new layer in a way that the next square looks back at the squares around it
    and finds the one with the greatest path so far. This way, each square is part of the best path it can
    be on.
    */
    Paths[0][Start] = Path(Start,Vals[Start], NULL);
    for (int MoveNum : range(1, MaxRange)){
        double ValMoveAdjust = GetValMoveAdjust(GetAdjRange(MoveNum, MoveRange));
        AttachPrevPaths(Paths[MoveNum - 1], Paths[MoveNum], ValMoveAdjust, Vals);
    }

    //sets the furthest outs PathVals (here , the PathVal == CumVal by definition)
    for (Path & LastPath : Paths[MaxRange - 1]){
        LastPath.PathVal = LastPath.CumVal;
    }
    /*
    With the assumption that the furthest out PathVal will contain the best paths (which may be false)
    this goes through backwards to find the best path that it can link to
    */
    for (int MoveNum : range(MaxRange - 1, 0, -1)){
        AttachCurPaths(Paths[MoveNum - 1], Paths[MoveNum], Start);
    }

    auto RetVal = ConvertToMovePaths(Paths, Start, MoveRange);
    return RetVal;
}

void MakePathVals(vector<Array2d<double> *> & InVals, vector<Array2d<double> *> & OutVals, Point Start, int MoveRange, int Moves){
    int Futures = (Moves - 1) * MoveRange + 1;//so that rounding up never gets passed the last movevector<Array2d<Path>> Paths(Moves * MoveRange);
    vector<Array2d<Path>> Paths(Futures);

    Paths[0][Start] = Path(Start, (*InVals[0])[Start], NULL);

    for (int SM : range(1, Futures)){
        int M = GetAdjRange(SM, MoveRange);
        AttachPrevPaths(Paths[SM - 1], Paths[SM], GetValMoveAdjust(M), *InVals[M]);//has constant value over movenum
    }
    for (Path & LastPath : Paths[Futures - 1]){
        LastPath.PathVal = LastPath.CumVal;
    }
    for (int SM : range(Futures - 1, 0, -1)){
        AttachCurPaths(Paths[SM - 1], Paths[SM], Start);//has constant value over movenum
    }

    for (int MoveNum : range(Moves)){
        int PNum = MoveRange * MoveNum;

        foreachArray2d(Paths[PNum], *OutVals[MoveNum], [&](Path & Pa, double & Val){
            Val = (Pa.PathVal > -10e7) * Pa.PathVal;
        });
    }
}
Array2d<double> SpreadVals(Array2d<double> & Vals){
    double TotVal = 0;
    for (double & Val : Vals)
        TotVal += (Val <= 0) ?
                    0 :
                    Val;

    Array2d<double> Spread;
    if (TotVal > 0){
        //multiplication is faster than division optimization
        double InvTotVal = 1.0 / TotVal;
        foreachArray2d(Spread, Vals, [&](double & SVal, double & Val){
            SVal = (Val <= 0) ?
                    0 :
                    (Val * InvTotVal);
        });
    }
    return Spread;
}
Array2d<double> EjectValsFromLand(TroopInfo<Array2d<double>> & TChance, Array2d<Domination> & PlayerDom, vector<EPlayer *> & AllPlayers, SimpleCompPlayer * This){
    //changes the land and returns the value of that change
    Array2d<double> Vals(0);
    unordered_map<EPlayer *, double> LandValue(AllPlayers.size());
    for (EPlayer * Play : AllPlayers)
        LandValue[Play] = GetValOfDom(Play);

    for (Point P : BoardIterate()){
        Domination & Dom = PlayerDom[P];
        double ThisLandVal =
            (Dom.Influence == MaximumDomination && Dom.Player > 0) ?
                    (OnSameTeam(This, AllPlayers[Dom.Player])) ?
                    0 :
                LandValue[AllPlayers[Dom.Player]] + LandValue[This] :
            LandValue[This];

        int AttackRange = TChance.T->Range;
        double TotalDomChange = 0;
        for (Point AP : SquareIterate(P, AttackRange)){
            double ThisChance = TChance.Info[AP];
            double ThisInfluence = GetDominationOfTroop(AttackRange, GetBoardDistance(P, AP));
            //increases the value of the spot that can change this value
            Vals[AP] += ThisChance * ThisLandVal * ThisInfluence / MaximumDomination;
            //changes the domination of the spot based on influence and chance
            TotalDomChange += ThisChance * ThisInfluence;
        }
        //finalizes domination change
        int DirectionalDomChange = TotalDomChange * ((Dom.Player == TChance.PlayerNum) ? 1 : -1);
        Dom.Influence = max(Dom.Influence + DirectionalDomChange, MaximumDomination);
    }
    return Vals;
}
Array2d<double> EjectValsFromBuildChances(Array2d<BuildInfo<double>> & BChance, TroopInfo<Array2d<double>> & TChance){
    //this takes the maximum value a building each TChance can attack and removes some of the BChance away
    //this function promotes drift!!! (some values become better than others because of position on an arbitrary iterator)
    //does not take into account blocking of other buildings right now, although that isn't really pressing
    if (TChance.T->TAttackType != TroopAndBuild)
        return Array2d<double>(0);
    Array2d<double> AdjBVals;
    foreachArray2d(BChance, AdjBVals, [&](BuildInfo<double> & BInf, double & BVal){
        BVal = (BInf.TeamNum == TChance.TeamNum || BInf.B == NULL) ?
            0 :
            ((BInf.Info * BInf.B->GetCost()) / BInf.B->GetSize());
    });
    Array2d<double> Vals(0);
    for (Point BP : BoardIterate()){
        double TCh = TChance.Info[BP];
        PointVal MaxVal(0,BP);
        for (Point P : SquareIterate(BP, TChance.T->Range)){
            double CurVal = AdjBVals[P];
            if (CurVal > MaxVal.Val)
                MaxVal = PointVal(CurVal, P);
        }
        if (MaxVal.Val < 0)
            continue;
        double ValOfChange = MaxVal.Val * TCh;
        //this is what is causing drift, but getting rid of it will lead to illogical stuff too
        AdjBVals[MaxVal.Info] = min(AdjBVals[MaxVal.Info] - ValOfChange, 0.0);
        //changes the raw chances
        BChance[MaxVal.Info].Info = min(BChance[MaxVal.Info].Info - TCh,0.0);
        //change the output values
        Vals[BP] = ValOfChange;
    }
    return Vals;
}
typedef TroopInfo<Array2d<double>> TroopChance;
typedef TroopInfo<Array2d<double>> TroopValues;
struct ValChance{
    double Val;
    double Chance;
};
typedef Array2d<vector<TroopInfo<ValChance>>> TroopCollection;
typedef DArraySlice<TroopInfo<Array2d<double>>> TChanceCollection;
typedef BuildInfo<double> BuildVal;
void Iterfeer(TroopChance & Att, TroopChance & Def, TroopValues & AVals, TroopValues & DVals,TroopValues & RawDVals){
    /*
    this troop affects the values of all of the other troops by taking the chance the offender will be in range with
    the defender, getting the estimated chance that the offender will beat the defender, getting the value of the
    defender, which is the value of the troop plus the value of the changes already in FutVals, and then subtracting
    the resulting value from the defensive vals and adding the value to the offensive vals
    */
    if (Att.TeamNum == Def.TeamNum)//if they are on the same team, they won't attack each other
        return;
    double Chance = GetChanceToWin(Att.T, Def.T);
    int DefaultDefVal = Def.T->GetValue();
    //at this point This is in the form of values as in money and Affect is in the form of probability of existence
    for (Point P : BoardIterate()){
        double & AC = Att.Info[P];
        double & DC = Def.Info[P];
        if (AC > 1e-10 && DC > 1e-10){
            double TotDefVal = DefaultDefVal + RawDVals.Info[P];//todo:fix this so that it represents actual values of future movements instead of this really weak value
            double AltVal = Chance * TotDefVal * AC * DC;
            //optimizes by taking out multiplication from indicies access
            FastCoordSquareIter(P, Att.T->Range, [&](int X, int Y){
                AVals.Info.Arr[X][Y] += AltVal;
                DVals.Info.Arr[X][Y] -= AltVal;
            });
        }
    }
}
void Iterfeer(TChanceCollection Chance, TChanceCollection OutVals, TChanceCollection SpotVals){
    /*
    this troop affects the values of all of the other troops by taking the chance the offender will be in range with
    the defender, getting the estimated chance that the offender will beat the defender, getting the value of the
    defender, which is the value of the troop plus the value of the changes already in FutVals, and then subtracting
    the resulting value from the defensive vals and adding the value to the offensive vals
    */
    for (int AtNum : range(Chance.Size)){
        auto & AInfo = Chance[AtNum];
        auto & ASpotVal = SpotVals[AtNum];
        auto & AOutVal = OutVals[AtNum];
        for (Point P : BoardIterate()){
            double AC = AInfo.Info[P];
            Troop * AttT = AInfo.T;
            if (AC <= 0)
                continue;
            ValInfo<int> BestDef;
            for (int DefNum : range(Chance.Size)){
                if (AInfo.TeamNum == Chance[DefNum].TeamNum)
                    continue;
                Troop * DefT = Chance[DefNum].T;
                double DBaseVal = DefT->GetValue();
                double TotVal = 0;
                for (Point AP : SquareIterate(P, AttT->Range)){
                    //it can only subtract
                    //todo: explore getting rid of base val and only using the accumulated value of the troop
                    TotVal += Chance[DefNum].Info[AP] * (SpotVals[DefNum].Info[AP] + DBaseVal);
                }
                double WinChance = GetChanceToWin(AttT, DefT);
                double TotDefVal = TotVal * WinChance;
                if (TotDefVal > BestDef.Val)
                    BestDef = ValInfo<int>(TotDefVal, DefNum);
            }
            if (BestDef.Val <= 0)
                continue;

            auto & DInfo = Chance[BestDef.Info];
            auto & DSpotVal = SpotVals[BestDef.Info];
            auto & DOutVal = OutVals[BestDef.Info];
            Troop * DefT = DInfo.T;

            double WinChance = GetChanceToWin(AttT, DefT);
            double DBaseVal = DefT->GetValue();
            double ChanceToRemove = AC * WinChance;
            for (Point AP : SquareIterate(P, AInfo.T->Range)){
                //chance to remove times chance it is there, times value of troop
                double ChangeVal = ChanceToRemove * DInfo.Info[AP] * (DBaseVal + DSpotVal.Info[AP]);
                AOutVal.Info[AP] += ChangeVal;
                DOutVal.Info[AP] -= ChangeVal;
            }
        }
    }
}
void Iterfeer(TChanceCollection Chance, TChanceCollection OutVals, TChanceCollection SpotVals, Array2d<BuildInfo<double>> & BChance){
    /*
    the OutVals and BChance are affected by taking the chance of the attacker, dividing it so that the maximum effect of attacking something is
    one and making
    */

    for (int AtNum : range(Chance.Size)){
        TroopChance & AInfo = Chance[AtNum];
        TroopValues & ASpotVal = SpotVals[AtNum];
        TroopValues & AOutVal = OutVals[AtNum];
        for (Point P : BoardIterate()){
            double AC = AInfo.Info[P];
            Troop * AttT = AInfo.T;
            if (AC <= 0)
                continue;
            double AllTotDefChance = 0;
            //adds in the chance of buildings being there and getting attacked
            if (AttT->TAttackType == TroopAndBuild){
                for (Point AP : SquareIterate(P, AttT->Range)){
                    BuildVal & BCha = BChance[AP];
                    if (BCha.TeamNum != AInfo.TeamNum && BCha.B != NULL)
                        AllTotDefChance += BCha.Info;
                }
            }
            for (int DefNum : range(Chance.Size)){
                TroopChance & DefChance = Chance[DefNum];

                if (AInfo.TeamNum == DefChance.TeamNum)
                    continue;

                double TotDefChance = 0;
                for (Point AP : SquareIterate(P, AttT->Range))
                    TotDefChance += DefChance.Info[AP];

                AllTotDefChance += TotDefChance * GetChanceToWin(AttT, DefChance.T);
            }
            if (AllTotDefChance <= 0)
                continue;
            /*the ChanceModifier makes sure that the total amount of damage the attacking troop inflicts cannot exceed 1
            troop per turn.*/
            double ChanceModifier = min(1.0, 1.0 / AllTotDefChance);

            if (AttT->TAttackType == TroopAndBuild){
                double BuildChanceToRemove = AC * ChanceModifier;
                for (Point AP : SquareIterate(P, AttT->Range)){
                    BuildVal & BCha = BChance[AP];
                    if (BCha.TeamNum != AInfo.TeamNum && BCha.B != NULL){
                        double BuildVal = double(BCha.B->GetCost()) / BCha.B->GetSize();
                        AOutVal.Info[AP] += BuildVal * BCha.Info * BuildChanceToRemove;
                        BCha.Info -= BuildChanceToRemove;
                    }
                }
            }
            for (int DefNum : range(Chance.Size)){
                TroopChance & DefChance = Chance[DefNum];
                TroopValues & DSpotVal = SpotVals[DefNum];
                TroopValues & DOutVal = OutVals[DefNum];
                Troop * DefT = DefChance.T;

                if (AInfo.TeamNum == DefChance.TeamNum)
                    continue;

                double DBaseVal = DefT->GetValue();

                double ChanceToRemove = AC * ChanceModifier * GetChanceToWin(AttT, DefT);

                for (Point AP : SquareIterate(P, AttT->Range)){
                    //chance to remove times chance it is there, times value of troop
                    double DC = DefChance.Info[AP];
                    if (DC > 0){
                        double TotDefVal = DBaseVal + DSpotVal.Info[AP];
                        double ChangeVal = ChanceToRemove * DC * TotDefVal;
                        AOutVal.Info[AP] += ChangeVal;
                        DOutVal.Info[AP] -= ChangeVal;
                    }
                }
            }
        }
    }
}
vector<TroopInfo<PartialRangeArray<ValInfo<vector<Point>>>>> SimpleCompPlayer::GetInteractingPaths(){
    /*
    starting with a constant value(of 1), the values of troop movement are calculated. The values divided by the
    total value is taken to be the percentage chance of the troop being on that square. This is then used to affect the
    chances of buildings being there there, of land being dominated by differnt troops, and of buildings existing. These
    affectings are easily translated to be the values of the troop moving there. This is then taken to be the
    */
    const int MaxMoves = 10;
    const int NumOfIters = 7;
#ifdef Debug_Macro_Move
    for (int g : range(4))
        VecTVals[g].resize(NumOfIters);
    VecBVals.resize_w_loss(NumOfIters, MaxMoves);
#endif
    ProtectedGlobal<Array2d<int>> Protectkey(PlayerOcc);
    //removes the spots of the moveable troops of all of the players
    for (Player * Play : AllPlayers){
        for (Troop * T : Play->Troops){
            if (T->MovementPoints > 0)
                PlayerOcc[T->Spot] = -1;
        }
    }
    //gets the total number of troops on the board
    int TroopSize = 0;
    for (EPlayer * Play : AllPlayers)
        TroopSize += Play->Troops.size();
    DArray2d<TroopInfo<Array2d<double>>> TPVals(MaxMoves, TroopSize), RawTPaths, SpreadTPaths, NextTPVals;
    //TPVals.		resize_w_loss(MaxMoves, TroopSize);
    /*RawTPaths.	resize_w_loss(MaxMoves, TroopSize);
    NextTPVals.	resize_w_loss(MaxMoves, TroopSize);
    SpreadTPaths.resize_w_loss(MaxMoves, TroopSize);*/

    //initializes the PathVals, the troops are ordered by thier turn, so that overlapping effects work closer to real life
    {
        int TNum = 0;
        for (int g : range(AllPlayers.size())){
            int PNum = (g + PlayerNum) % AllPlayers.size();
            EPlayer * Play = AllPlayers[PNum];
            for (Troop * T : Play->Troops){
                Array2d<double> DefVals(1);
                for (int M : range(MaxMoves)){
                    TPVals[M][TNum] = MakeTInfo(T, Play->TeamNum, Play->PlayerNum, DefVals);
                }
                TNum += 1;
            }
        }
    }
    RawTPaths = SpreadTPaths = NextTPVals = TPVals;

    for (int Iter : range(NumOfIters)){
        //at this point the values are stored in TPVals. Here they are converted into likelyhood of placement and put into TPaths
        for (int TNum : range(TroopSize)){
            vector<Array2d<double> *> PathVec(MaxMoves);
            vector<Array2d<double> *> ValVec(MaxMoves);
            for (int MoveNum : range(MaxMoves)){
                ValVec[MoveNum] = &TPVals[MoveNum][TNum].Info;
                PathVec[MoveNum] = &RawTPaths[MoveNum][TNum].Info;
            }
            Troop * T = RawTPaths[0][TNum].T;
            MakePathVals(ValVec, PathVec, T->Spot, T->MovementPoints, MaxMoves);
        }

        Array2d<Domination> FutureDom = PlayerDom;
        Array2d<BuildInfo<double>> BuildChance;
        //initialized BuildChance to 1 for all places with buildings
        for (Point P : BoardIterate()){
            BuildChance[P] = (GetOccType(P) == BuildingType) ?
                BuildInfo<double>{GetBuilding(P), GetPlayer(P)->TeamNum, 1.0} :
                BuildInfo<double>{ NULL, -1, 0.0 };
        }

        //puts values into NextTPathVals
        for (int MoveNum : range(MaxMoves)){
            for (int TNum : range(TroopSize)){
                auto & TPath = SpreadTPaths[MoveNum][TNum];
                TPath.Info = SpreadVals(RawTPaths[MoveNum][TNum].Info);
                auto & FutVals = NextTPVals[MoveNum][TNum];
                FutVals.Info.Init(0);

                //the values of the troop affecting the land/buildings are recorded in FutVals and simeltaniouly changed
                //then the values are decreased so that later troops/turns don't benefit from the same values of changes
                FutVals.Info += EjectValsFromLand(TPath, FutureDom, AllPlayers, this);
                //put this in buildstuff
                FutVals.Info += EjectValsFromBuildChances(BuildChance, TPath);
            }
            //for (int TNum : range(TroopSize)){
            //	for (int DefTNum : range(TroopSize)){
            //		//this also takes into account defending other troops (if you have gotten around to implementing path value recording!)
            //		Iterfeer(SpreadTPaths[MoveNum][TNum], SpreadTPaths[MoveNum][DefTNum],
            //			NextTPVals[MoveNum][TNum], NextTPVals[MoveNum][DefTNum],
            //			RawTPaths[MoveNum][DefTNum]);
            //	}
            //}
            //put buildchances in here as well and incorperate it into the same system.
            Iterfeer(SpreadTPaths[MoveNum], NextTPVals[MoveNum], RawTPaths[MoveNum]);
            for (int TNum : range(TroopSize)){
                //averages the current/future path values with the previous path values
                NextTPVals[MoveNum][TNum].Info += TPVals[MoveNum][TNum].Info;
                NextTPVals[MoveNum][TNum].Info *= Array2d<double>(0.5);//TPVals is the sum of two vals and so must be divided by 2, or multiplied by 0.5
            }
#ifdef Debug_Macro_Move
            for (Point P : BoardIterate())
                VecBVals[Iter][MoveNum][P] = BuildChance[P].Info;
#endif
        }
#ifdef Debug_Macro_Move
        VecTVals[0][Iter] = TPVals;
        VecTVals[1][Iter] = NextTPVals;
        VecTVals[2][Iter] = SpreadTPaths;
        VecTVals[3][Iter] = RawTPaths;
#endif
        TPVals = NextTPVals;
    }
    //the values are still stored in NextTPathVals, now we need to convert them to paths and then to MovePaths
    vector<TroopInfo<PartialRangeArray<ValInfo<vector<Point>>>>> RetVals(Troops.size());
    for (int TNum : range(Troops.size())){//this players troops should be the the first troops up to Troops.size()
        vector<Array2d<double> *> ValPtrs(MaxMoves);
        for (int MoveNum : range(MaxMoves)){
            ValPtrs[MoveNum] = &TPVals[MoveNum][TNum].Info;
        }
        TroopInfo<Array2d<double>> & TInf = TPVals[0][TNum];
        BoardSquare TSq(TInf.T->Spot, TInf.T->MovementPoints);
        vector<Array2d<Path>> Spread(MaxMoves * TInf.T->MovementPoints);
        Point Start = TInf.T->Spot;
        int MoveRange = TInf.T->MovementPoints;
        int Futures = MaxMoves * MoveRange;
        Futures -= (Futures - 1) % MoveRange;
        Spread[0][Start] = Path(Start, (*ValPtrs[0])[Start], NULL);

        for (int SM : range(1, Futures)){
            int M = RoundUpRatio(SM, MoveRange);
            AttachPrevPaths(Spread[SM - 1], Spread[SM], 1.0, *ValPtrs[M]);//has constant value over movenum
        }
        for (Path & LastPath : Spread[Futures - 1]){
            LastPath.PathVal = LastPath.CumVal;
        }
        for (int SM : range(Futures - 1, 0, -1)){
            AttachCurPaths(Spread[SM - 1], Spread[SM], Start);//has constant value over movenum
        }
        RetVals[TNum] = MakeTInfo(TInf.T, TInf.TeamNum, TInf.PlayerNum, ConvertToMovePaths(Spread,TSq.Cen,TSq.Range));
    }
    return RetVals;
}
