#include "stdafx.h"
#include "ComputerPlayer.h"
/*
This file contains functions which help MoveTroop in doing the following actions:

Finds the basic movement and attack value of every troop of every group
Movement: Determined using the values from GetTroopValues and GetPaths and dived by 1000 to show the difference in order of
magnitude of importance (This may not be the best action considering this is the only way to currently simulate multiple moves).
Attack: Decreases value depending on (Number of enemy troops that can attack) / (Number of friendly troops each enemy troop can attack) *
value of ThisTroop.
Increase value depending on (sum of value of troop this thing can attack)/(Num of troops this thing can attack)

Then, it changes the value depending on the result of the
*/
void copy_into(MoveSquareVal & src, MoveSquareVal & dest) {
    dest.MoveV = src.MoveV;
    copy_n(src.AttackV.Arr.begin(), dest.AttackV.Arr.size(), dest.AttackV.Arr.begin());
}
void copy_into(MoveSquareVals & src, MoveSquareVals & dest) {
    for_each_range(src.Arr, dest.Arr, [&](MoveSquareVal & src_v, MoveSquareVal & dest_v) {
        copy_into(src_v, dest_v);
    });
}
void copy_into(GroupSquareVals & src, GroupSquareVals & dest) {
    for_each_range(src, dest, [&](pair< Troop *const , MoveSquareVals> & src_p, pair<Troop *const, MoveSquareVals> & dest_p) {
        copy_into(src_p.second, dest_p.second);
    });
}
template<typename Fnc>
void SimpleCompPlayer::IterOverMove(Troop * T, Fnc FIter) {
    int MoveRange = T->MovementPoints;
    for (Point MoveP : SquareIterate(T->Spot, T->MovementPoints)) {
        int TempMoveRange = MoveRange;
        if (BlankPoint(MoveP) == true && CheckMovement(T->Spot, MoveP, TempMoveRange) == true)//CheckMovement alters the value of MoveRange, watch out!!!
            FIter(MoveP);
    }
}
template<typename Fnc>
void SimpleCompPlayer::IterOverAttack(Troop * T, Point MoveP, Fnc FIter) {
    int Range = T->Range;
    for (Point AttcP : SquareIterate(MoveP, Range)) {
        if (CheckAttack(MoveP, AttcP, Range) == true)
            FIter(AttcP);
    }
}
template<typename Fnc>
void SimpleCompPlayer::IterOverMoveAttack(Troop * T, Fnc FIter) {
    IterOverMove(T, [&](Point MoveP) {
        IterOverAttack(T, MoveP, [&](Point AttackP) {
            FIter(AttackP);
        });
    });
}
int GetSpread(Troop * T, Point Spot, int OverRange, Array2d<SquareThreat> & MyThreat) {
    int Spread = 0;
    for (Point P : SquareIterate(Spot, OverRange))
        if (MyThreat[P].ThreatTroops.count(T))//if the troop is threatening that square is there
            Spread++;
    return Spread;
};
AttackInfo GetMyBestAttack(GroupSquareVals & TVals) {
    ValInfo<AttackInfo> BestAttack;
    for (auto & TValPair : TVals)
        for (auto MoveVals : TValPair.second)
            for (auto AttackVals : MoveVals.Info().AttackV)
                if (BestAttack.Val < AttackVals.Info())
                    BestAttack = ValInfo<AttackInfo>(AttackVals.Info(), { MoveVals.P, AttackVals.P, TValPair.first });

    return BestAttack.Info;
};
template<typename Fnc>
void SimpleCompPlayer::IterThroughTValBlockedAttacks(Point MoveP, Troop * ThisT, GroupSquareVals & IterVals, Fnc Fn) {
    auto Protectkey = Protect(PlayerOcc[MoveP]);
    PlayerOcc[MoveP] = PlayerNum;
    try {
        for (auto & PairVal : IterVals) {
            Troop * T = PairVal.first;
            if (T == ThisT)
                continue;
            if (GetBoardDistance(MoveP, T->Spot) > T->GetMoveAttackRange())
                continue;

            for (auto MovePair : PairVal.second) {
                Point ThisMP = MovePair.P;
                AttackSquareVals & MoveVals = MovePair.Info().AttackV;

                if (GetBoardDistance(MoveP, ThisMP) > T->Range)
                    continue;

                for (auto AttackPair : MoveVals) {
                    if (!CheckAttack(ThisMP, AttackPair.P, T->Range))
                        Fn(AttackPair.Info());
                }
            }
        }
    }
    catch (...) {}//allows breaking out of loop using throw
};
template<typename Fnc>
void SimpleCompPlayer::IterThroughTValBlockedMoves(Point MoveP, GroupSquareVals & IterVals, Fnc Fn) {
    //global dependence, no throwing!!
    ProtectedGlobal<int> ProtectKey(PlayerOcc[MoveP]);
    PlayerOcc[MoveP] = PlayerNum;
    try {
        for (auto TPair : IterVals) {//erases values of places troops can no longer move
            Troop * T = TPair.first;
            if (GetBoardDistance(T->Spot, MoveP) > T->MovementPoints)//if it is cannot interfeer because it is too far
                continue;
            for (auto MPair : TPair.second) {
                Point ThisMP = MPair.P;
                int TempMovePoint = T->MovementPoints;
                if (MoveP == ThisMP || CheckMovement(T->Spot, MPair.P, TempMovePoint) == false) {
                    Fn(MPair.Info());
                }
            }
        }
    }
    catch (...) {}//allows breaking out of loop using throw
};
template<typename Fnc>
void SimpleCompPlayer::IterThroughAttackSquaresOfTroops(Point AttackP, Troop * ThisT, GroupSquareVals & IterVals, Fnc Fn) {
    for (Troop * MyT : AntiThreat[AttackP].ThreatTroops) {//those are the troops that are theoretically within range of the square
        if (MyT == ThisT)
            continue;
        for (auto MovePair : IterVals[MyT]) {
            AttackSquareVals & AVals = MovePair.Info().AttackV;
            if (AVals.IsInScope(AttackP)) {
                Fn(AVals[AttackP]);
            }
        }
    }
};
template<typename Fnc>
void IterThroughMovesOnSquare(Point MoveP, Troop * ThisT, GroupSquareVals & IterVals, Fnc Fn) {
    for (auto & TPair : IterVals) {
        MoveSquareVals & MVals = TPair.second;
        if (TPair.first != ThisT && MVals.IsInScope(MoveP))
            Fn(MVals[MoveP]);
    }
}
template<typename Fnc>
void SimpleCompPlayer::IterThroughBlockingMoves(AttackInfo MoveInfo, GroupSquareVals & IterVals, Fnc Fn) {
    try {
        for (auto & TPair : IterVals) {
            Troop * T = TPair.first;
            MoveSquareVals & MoveVals = TPair.second;
            //a crude and imperfect check on whether this troop can block this attack
            auto CrudeCheckAttack = [&](Point Spot) {
                return GetBoardDistance(Spot, T->Spot) > T->MovementPoints + MoveInfo.T->Range;
            };
            if (CrudeCheckAttack(MoveInfo.AttackP) || CrudeCheckAttack(MoveInfo.MoveP))
                continue;
            for (auto MovePair : MoveVals) {
                Point MoveP = MovePair.P;
                ProtectedGlobal<int> Key(PlayerOcc[MoveP]);
                PlayerOcc[MoveP] = PlayerNum;
                if (!CheckAttack(MoveInfo.MoveP, MoveInfo.AttackP, MoveInfo.T->Range)) {
                    for (double & MoveAttackVals : MovePair.Info().AttackV.Arr)
                        Fn(MoveAttackVals);
                }
            }
        }
    }
    catch (...) {}//allows breaking out of loop using throw
}
double GetN(MoveSquareVals & Vals) {
    return Vals.ActiveSize() * (*Vals.begin()).Info().AttackV.Size();//the size of the partialrangearray times the size of all of its rangearrays
}
double GetSum(MoveSquareVals & Vals) {
    double Sum = 0;
    for (auto MovePair : Vals)
        for (double & AttackVal : MovePair.Data->AttackV.Arr)
            Sum += AttackVal;
    return Sum;
}
//the zero functions get the values if the mean is considered to be zero and all negative numbers are also zero
double GetTotVariance(MoveSquareVals & Vals) {
    double Mean = GetSum(Vals) / GetN(Vals);
    double TotVar = 0;
    for (auto MovePair : Vals)
        for (double & AttackVal : MovePair.Data->AttackV.Arr)
            TotVar += Square(Mean - AttackVal);
    return TotVar;
}

void MakeAdjVals(GroupSquareVals & TVals, GroupSquareVals & AdjVals) {
    copy_into(TVals, AdjVals);
    const double min_sum = 5e-40;//needs to be sufficiently high to stop numeric overflow when dividing TotVariance
    //Turns AdjVals into (val-AvVal)/maxZScore
    for (auto & TroopPair : AdjVals) {
        MoveSquareVals & Vals = TroopPair.second;

        int N = GetN(Vals);

        double AMean = GetSum(Vals) / GetN(Vals);
        double ATotVar = GetTotVariance(Vals);

        double MSum = 0;
        for (auto MovePair : Vals)
            MSum += MovePair.Data->MoveV;
        double MMean = MSum / Vals.Size();
        double MTotVar = 0;
        for (auto MovePair : Vals)
            MTotVar += Square(MMean - MovePair.Data->MoveV);

        //AdjVals are being set using adjustments on their own values (remember they are copied from TVals)
        for (auto MovePair : Vals) {
            double & MVal = MovePair.Data->MoveV;
            double MDif = MVal - MMean;
            double MVarience = Square(MDif);

            MVal = (MDif > min_sum) ? MVarience / MTotVar : 0.0;

            for (double & AttackVal : MovePair.Data->AttackV.Arr) {
                //the SD is actually in terms of the regular standard deviation
                double ADif = AttackVal - AMean;
                double AVarience = Square(ADif);
                AttackVal = ADif > min_sum ? AVarience / ATotVar : 0.0;
            }
        }
    }
}
//void MakeAdjVals(GroupSquareVals & TVals, GroupSquareVals & AdjVals){
//	AdjVals = TVals;
//	//Turns AdjVals into chance of being in the spot
//	for (auto & TroopPair : AdjVals){
//		MoveSquareVals & Vals = TroopPair.second;
//		//AdjVals are being set using adjustments on their own values (remember they are copied from TVals)
//		for (auto & MovePair : Vals){
//			double Sum = sum(MovePair.Data->Arr);
//			for (double & AttackVal : MovePair.Data->Arr){
//
//				//AttackVal = max(0.0, NewVal);//lowest value is zero
//			}
//		}
//	}
//}
void SimpleCompPlayer::SubtractBlockingMoves(GroupSquareVals & TVals, int NumOfIters) {
    if (TVals.size() == 0)
        return;

    //actually changes the vals
    GroupSquareVals AdjVals = TVals;
#ifdef Debug_Macro_Move
    VecMVals.resize(NumOfIters);
#endif
    for (int N : range(NumOfIters)) {
#ifdef Debug_Macro_Move
        VecMVals[N].resize(TVals.size());
        int M = 0;
        for (auto & PRA : TVals) {
            VecMVals[N][M] = PRA.second;
            M++;
        }
#endif
        MakeAdjVals(TVals, AdjVals);
        SubtractBlockingMove(TVals, AdjVals);
    }
}
void SimpleCompPlayer::SubtractBlockingMove(GroupSquareVals & TVals, GroupSquareVals & AdjVals) {
    for (auto & TroopPair : TVals) {
        Troop * MyTroop = TroopPair.first;
        for (auto MovePair : TroopPair.second) {
            for (auto AttackPair : MovePair.Info().AttackV) {
                double & AttackVal = AttackPair.Info();
                IterThroughAttackSquaresOfTroops(AttackPair.P, MyTroop, AdjVals, [&](double & Val) {
                    AttackVal -= AttackVal * Val;
                });
            }
            Point MoveP = MovePair.P;
            double & MoveVal = MovePair.Data->MoveV;
            //goes through the spots that this movement blocks attackwise and movewise and changes all of the values in the move by this ammound
            //note that the values of the blocked attacks and the blocked moves can be the same, which may not be good
            /*IterThroughTValBlockedAttacks(MoveP, MyTroop, AdjVals, [&](double & Val) {
                MoveVal -= MoveVal * Val;
            });*/
            //Iterates through the spots that this thing blocks Movewise
            IterThroughMovesOnSquare(MoveP, MyTroop, AdjVals, [&](MoveSquareVal & Vals) {
                MoveVal -= MoveVal * (max(Vals.AttackV.Arr) + Vals.MoveV);
            });
            //this may not be a good implementation of an move blocker
            /*IterThroughTValBlockedMoves(MoveP, TVals, [&](RangeArray<double> & Vals){
            double MaxVal = 0;
            for (double & AttcVal : Vals.Arr)
            MaxVal = max(MaxVal, AttcVal);

            ChangeInMoveVal -= MaxVal;
            });*/
        }
    }
}
//Array2d<double> SimpleCompPlayer::GetGroupVals(Group * G){
//	struct Path{
//		Point P;
//
//		Path * LastPath;
//		double CumVal;
//
//		Path * NextPath;
//		double PathVal;
//
//		Path(){ LastPath = NULL; NextPath = NULL; CumVal = -10e50; PathVal = -10e50; }
//		Path(Point InP,double AddVal, Path * InLastPath){
//			P = InP;
//			LastPath = InLastPath;
//			CumVal = AddVal;
//			NextPath = NULL;
//			if (LastPath != NULL)
//				CumVal += LastPath->CumVal;
//		};
//		void PushPath(vector<Point> & OutPath){
//			OutPath.push_back(P);
//			if (NextPath != NULL)
//				NextPath->PushPath(OutPath);
//		}
//	};
//	const int MaxRange = max(BoardSizeX, BoardSizeY);
//	if (MoveRange == 0){
//		throw "Getpaths doesn't work for range 0 troops!";
//	}
//	Array2d<int> SavedPlayerOcc = PlayerOcc;
//	for (EPlayer * Play : AllPlayers){
//		for (Troop * T : Play->Troops)
//			if (T->MovementPoints > 0)
//				PlayerOcc[T->Spot] = -1;
//	}
//
//	vector<Array2d<Path>> Paths(MaxRange);//the paths are to their neighbors that led to them
//	//sets the original point in the vector
//	if (!BlankPoint(Start))
//		throw "Start needs to be empty for GetPaths to work properly";
//	/*
//	This creates each new layer in a way that the next square looks back at the squares around it
//	and finds the one with the greatest path so far. This way, each square is part of the best path it can
//	be on.
//	*/
//	Paths[0][Start] = Path(Start,Vals[Start], NULL);
//	for (int MoveNum : range(1,MaxRange)){
//		Array2d<Path> & PrevPaths = Paths[MoveNum - 1];
//		Array2d<Path> & CurPaths = Paths[MoveNum];
//		double ValMoveAdjust = 1.0 / GetAdjRange(MoveNum,MoveRange);
//		for (Point CurP:BoardIterate()){
//			if (!BlankPoint(CurP))
//				continue;
//			Path * BestPrevPath = &PrevPaths[CurP];//sets an arbitrary starting point
//			for (Point PrevP:SquareIterate(CurP, 1)){
//				if (PrevPaths[PrevP].CumVal > BestPrevPath->CumVal)
//					BestPrevPath = &PrevPaths[PrevP];
//			}
//			if (BestPrevPath->CumVal > -1000000.0)//if there is an active square that works
//				CurPaths[CurP] = Path(CurP,Vals[CurP] * ValMoveAdjust,BestPrevPath);//CurPath is not yet initialized
//		}
//	}
//	//sets the furthest outs PathVals (here , the PathVal == CumVal by definition)
//	for (Path & LastPath : Paths[MaxRange-1]){
//		LastPath.PathVal = LastPath.CumVal;
//	}
//	/*
//	With the assumption that the furthest out PathVal will contain the best paths (which may be false)
//	this goes through backwards to find the best path that it can link to
//	*/
//	for (int MoveNum : range(MaxRange - 1,0,-1)){
//		Array2d<Path> & CurPaths = Paths[MoveNum - 1];
//		Array2d<Path> & NextPaths = Paths[MoveNum];
//		double ValMoveAdjust = 1.0 / GetAdjRange(MoveNum-1,MoveRange);//ValMoveAdjust is for the CurPaths
//		for (Point CurP : BoardIterate()){
//			if (!BlankPoint(CurP))
//				continue;
//			Path * BestNextPath = &NextPaths[CurP];//sets an arbitrary starting point
//			for (Point NextP : SquareIterate(CurP, 1)){
//				if (NextPaths[NextP].PathVal > BestNextPath->PathVal)
//					BestNextPath = &NextPaths[NextP];
//			}
//			if (BestNextPath->LastPath != NULL || BestNextPath->P == Start){//if there is an active square that works
//				Path & CurPath = CurPaths[CurP];
//				CurPath.NextPath = BestNextPath;
//				//Since the PathVal = the best ones added up, this takes the PathVal - BestOne + ThisOne to find the value of this path
//				CurPath.PathVal = BestNextPath->PathVal - (BestNextPath->LastPath->CumVal - CurPath.CumVal);
//			}
//		}
//	}
//	//returns the useful values in a more sensible container, a blocked out 2d array around the MoveRange
//	PartialRangeArray<ValInfo<vector<Point>>> PathVals(Start,MoveRange);
//	Array2d<uint16_t> PathNums = FindMoveDistances(Start, MoveRange, MoveRange);
//	for (int MoveNum : range(1,MoveRange + 1)){//interates up to and including the MoveRange,starting from 1
//		Array2d<Path> & CurPaths = Paths[MoveNum];
//		//iterates where the shortest path for this number of moves is
//		for (Point MoveP : BoardIterate()){
//			if (MoveNum != PathNums[MoveP])
//				continue;
//			PathVals.SetExist(MoveP, true);
//			ValInfo<vector<Point>> & ThisVal = PathVals[MoveP];
//			ThisVal.Val = CurPaths[MoveP].PathVal;
//			CurPaths[MoveP].PushPath(ThisVal.Info);//ThisVal = the path
//		}
//	}
//	PlayerOcc = SavedPlayerOcc;
//	return PathVals;
//}
GroupSquareVals SimpleCompPlayer::GetValsOfGroupMove(Group * G) {
    //initlalizes the working parts of TVals and sets EnTroops
    set<Troop *> EnTroops;
    GroupSquareVals TVals;
    //it clears the troops that can move because the troop can take others' place, so they shouldn't block each other
    auto ProtectKey = Protect(PlayerOcc);
    RemoveMoveableTroops();

    //constructs TVals and EnTroops
    for (Troop * MyTroop : G->Troops) {

        MoveSquareVals & MoveVals = TVals[MyTroop];
        MoveVals = MoveSquareVals(MyTroop->Spot, MyTroop->MovementPoints);

        IterOverMove(MyTroop, [&](Point MoveP) {
            MoveVals.SetExist(MoveP, true);//if it can move there, yes, it does exist

                                           //inserts troops that can attack our troops when they move here
            InsertSet(EnTroops, Threat[MoveP].ThreatTroops);

            BoardSquare MoveSquare(MoveP, MyTroop->Range);
            AttackSquareVals & AttackVals = MoveVals[MoveP].AttackV;
            AttackVals = RangeArray<double>(MoveSquare);

            for (Point AttackP : SquareIterate(MoveSquare)) {
                //adds enemy troops that our troops can attack
                if (!BlankPoint(AttackP) && !OnSameTeam(GetPlayer(AttackP), this))//if there is an enemy player occupying the spot
                    if (GetOccType(AttackP) == TroopType)//if it is a troop that is there
                        EnTroops.insert(GetTroop(AttackP));
            }
        });
    }
    ProtectKey.Release();
    //sets vals to AttackVal / ThisSpread - Val * Sum(EnemyAttack/EnSpread)
    auto AllPathVals = GetInteractingPaths();
    int TNum = 0;
    for (Troop * MyTroop : G->Troops) {
        auto TPathVals = AllPathVals[TNum].Info;
        TNum += 1;
        for (auto MovePair : TVals[MyTroop]) {
            Point MoveP = MovePair.P;

            double MoveVal = (TPathVals[MoveP].Val / 1000.0);//starts out with macro val/1000 because it is totally too high right now
                                                             //subtracts the value of being killed
            MovePair.Info().MoveV = MoveVal;
            //for (Troop * EnT : Threat[MoveP].ThreatTroops)
            //	MoveVal -= MyTroop->GetValue() / GetSpread(EnT, EnT->Spot, EnT->GetMoveAttackRange(), Threat);

            for (auto AttackPair : MovePair.Info().AttackV){
                Point AttackP = AttackPair.P;
                //adds on the value of killing others
                if (!BlankPoint(AttackP) && GetPlayer(AttackP)->TeamNum != this->TeamNum)
                    *AttackPair.Data = GetValOfAttack(AttackP, MyTroop);
            }
        }
    }
    return TVals;
}
void SimpleCompPlayer::MoveGroup(Group * MoveG) {
    set<Troop *> FinishedTroops;
    set<Troop *> FailedTroops;

    GroupSquareVals TroopVals = GetValsOfGroupMove(MoveG);

    //second and nth iterations
    SubtractBlockingMoves(TroopVals, 10);
    double Val = 0;
    auto ProtectVal = Protect(Val);
    auto NewPV = std::move(ProtectVal);
    auto GetBest = [&](bool DoFailed) {
        ValInfo<AttackInfo> Best;
        for (auto & TroopPair : TroopVals) {
            Troop * MyTroop = TroopPair.first;
            if (FinishedTroops.count(MyTroop) || (!DoFailed && FailedTroops.count(MyTroop)))//if the troop is finished
                continue;
            MoveSquareVals & TMoveVals = TroopPair.second;
            for (auto MovePair : TMoveVals) {
                double MoveVal = MovePair.Data->MoveV;
                for (auto AttackPair : MovePair.Info().AttackV) {
                    ValInfo<AttackInfo> This(AttackPair.Info() + MoveVal, { MovePair.P,AttackPair.P,MyTroop });
                    if (This.Val > Best.Val)
                        Best = This;
                }
            }
        }
        return Best;
    };
    auto ClearDueToMove = [&](AttackInfo Info) {
        IterThroughTValBlockedAttacks(Info.MoveP, Info.T, TroopVals, [&](double & ThisAttackVal) {
            ThisAttackVal = -10e100;
        });
        IterThroughAttackSquaresOfTroops(Info.MoveP, Info.T, TroopVals, [&](double & ThisAttackVal) {
            ThisAttackVal = -10e100;
        });
        IterThroughBlockingMoves(Info, TroopVals, [&](double & ThisMoveAttackVals) {
            ThisMoveAttackVals = -10e100;
        });
    };
    //goes through the troops and if they cannot get to their preferred spot, then don't go anywhere for now
    while (true) {
        ValInfo<AttackInfo> BestAttackVal = GetBest(false);
        if (BestAttackVal.Val < -1000000.0)
            break;
        AttackInfo BestAttack = BestAttackVal.Info;
        //if the attack spot is blank, then it means there is no attacking going on and it should be ignored
        if (!BlankPoint(BestAttack.AttackP) && !CheckAttack(BestAttack.MoveP, BestAttack.AttackP, BestAttack.T->Range)) {
            //if the attack is currently blocked then it is a failed troop
            FailedTroops.insert(BestAttack.T);
            continue;
        }
        if (!MoveTroop(PointerToNum(Troops, BestAttack.T), BestAttack.MoveP)) {//actually moves the troop
                                                                               //it the troop moveing failed then it is a failed troop
            FailedTroops.insert(BestAttack.T);
            continue;
        }
        //now the troop is in a new spot
        ClaimedAttackSpots[BestAttack.T] = BestAttack.AttackP;
        FinishedTroops.insert(BestAttack.T);
        ClearDueToMove(BestAttack);
    }
    while (true) {
        ValInfo<AttackInfo> BestAttackVal = GetBest(true);
        if (BestAttackVal.Val < -1000000.0)
            break;
        AttackInfo BestAttack = BestAttackVal.Info;

        if (!BlankPoint(BestAttack.AttackP) && !CheckAttack(BestAttack.MoveP, BestAttack.AttackP, BestAttack.T->Range)) {
            //if the attack is currently blocked then the attack location is bad
            TroopVals[BestAttack.T][BestAttack.MoveP].AttackV[BestAttack.AttackP] = -1e100;
            continue;
        }
        if (!MoveTroop(PointerToNum(Troops, BestAttack.T), BestAttack.MoveP)) {//actually moves the troop
                                                                               //it the troop moveing failed then the move location is bad
            TroopVals[BestAttack.T].SetExist(BestAttack.MoveP, false);
            int Num = TroopVals[BestAttack.T].PointToInt(BestAttack.MoveP);
            continue;
        }
        //now the troop is in a new spot, MoveTroop moved it
        ClaimedAttackSpots[BestAttack.T] = BestAttack.AttackP;
        FinishedTroops.insert(BestAttack.T);
        ClearDueToMove(BestAttack);
    }
}
