
#include "stdafx.h"
#include "Base.h"

int PlayerTurn;
vector<Player *> Players;
int SRandVal = 1;

void SaveGameInfo(LPCWSTR FileName){
    ofstream SaveFile(FileName, ios::binary);
    //global variables PlayerTurn
    SaveFile.write((char*)(&PlayerTurn), sizeof(PlayerTurn));
    SaveFile.write((char*)(&PlayerOcc), PlayerOcc.Size());
    SaveFile.write((char*)(&PlayerDom), PlayerDom.Size());
    int NumOfPlayers = Players.size();
    SaveFile.write((char*)(&NumOfPlayers), sizeof(NumOfPlayers));
    for (Player * P : Players){
        P->Save(SaveFile);
    }
    SaveFile.close();
}
bool LoadGameInfo(LPCWSTR FileName){
    ifstream SaveFile(FileName, ios::binary);
    if (!SaveFile){
        return false;
    }
    //global variables
    SaveFile.read((char*)(&PlayerTurn), sizeof(PlayerTurn));//makes it a single char value, possibly incorrect
    SaveFile.read((char*)(&PlayerOcc), PlayerOcc.Size());
    SaveFile.read((char*)(&PlayerDom), PlayerDom.Size());
    int NumOfPlayers;
    SaveFile.read((char*)(&NumOfPlayers), sizeof(NumOfPlayers));
    Players.resize(NumOfPlayers);
    for (Player * P : Players){
        P->Load(SaveFile);
    }
    SaveFile.close();
    return true;
}
void ResetGame(){
    const int StartNumOfLFarms = 2;
    const int StartNumOfSFarms = 2;
    const int DisAwayFromEdge = 4;
    int NumOfPlayers = Players.size();
    for (Player * Play : Players)
        Play->StartNew();

    PlayerOcc.Init(-1);
    PlayerDom.Init(Domination{ -1, 0 });

    int XMinDisAway = BoardSizeX / (sqrt(NumOfPlayers) + 1);
    int YMinDisAway = BoardSizeY / (sqrt(NumOfPlayers) + 1);
    vector<Point> StartCenters;
    //gets the startcenters
    for (int p : range(NumOfPlayers)){
        for (int trys : range(100)){//attemps this 100 times
            int xspot = DisAwayFromEdge + rand() % (BoardSizeX - DisAwayFromEdge * 2);
            int yspot = DisAwayFromEdge + rand() % (BoardSizeY - DisAwayFromEdge * 2);
            bool TooClose = false;
            for (Point SC : StartCenters){
                if (abs(SC.X - xspot) < XMinDisAway && abs(SC.Y - yspot) < YMinDisAway)
                    TooClose = true;
            }
            if (!TooClose){
                StartCenters.push_back(CreatePoint(xspot, yspot));
                break;
            }
        }
    }
    if (StartCenters.size() != NumOfPlayers)
        throw "Board Initialization error!";
    //places the buildings
    for (int p : range(NumOfPlayers)){
        Point StartP = StartCenters[p];
        for (Point P : RectIterate(StartP.X - XMinDisAway / 2, StartP.Y - YMinDisAway / 2, StartP.X + XMinDisAway / 2, StartP.Y + YMinDisAway / 2)){
            PlayerDom[P].Player = p;
            PlayerDom[P].Influence = MaximumDomination;
        }

        int XCen = StartCenters[p].X;
        int YCen = StartCenters[p].Y;
        Players[p]->PlaceNewBuilding(TrainCenter, XCen, YCen, XCen + 1, YCen + 1);//this should theortically workevery time
        Players[p]->Buildings[0]->ResetStats();

        auto BuildStuff = [&](BuildingTypes B, int NumOfBuilds) {
            int Counter = 0;
            FastExpandingIter(StartCenters[p], 0, (XMinDisAway + YMinDisAway), [&](Point IterP){
                if (Players[p]->PlaceNewBuilding(B, IterP.X, IterP.Y, IterP.X + 1, IterP.Y + 1))
                    Counter++;
                if (Counter >= NumOfBuilds)
                    throw 0;//break out of loop
            });
            return;
        };
        //build LFarms
        BuildStuff(LFarm, StartNumOfLFarms);
        //Build SFarms
        BuildStuff(SFarm, StartNumOfSFarms);
    }
    //does the AI turn if it is the first troop to go
    Players[PlayerTurn]->DoAITurn(Players);
}
void AttackLocation(Point Loc, Troop * AttackingTroop){//decleration is in globals.h
    int PlayerNum = PlayerOcc[Loc];
    int AttackPlayerNum = PlayerOcc[AttackingTroop->Spot];
    //if they are on the same team, the attack fails
    if (OnSameTeam(Players[PlayerNum], Players[AttackPlayerNum]) == false)
        Players[PlayerNum]->PlayerAttackLocation(Loc, AttackingTroop);
}
bool AttemptAttack(Point Loc, Troop * AttackingTroop){
    int PlayerNum = PlayerOcc[Loc];
    int AttackPlayerNum = PlayerOcc[AttackingTroop->Spot];
    //if they are on the same team, the attack fails
    if (OnSameTeam(Players[PlayerNum], Players[AttackPlayerNum]))
        return false;
    else
        return Players[PlayerNum]->AttemptAttackOnSelf(Loc, AttackingTroop);
}
void ChangeOverallTurn(){
    int NumOfPlayers = Players.size();
    Player * OldPlayer = Players[PlayerTurn];
    OldPlayer->ResetStats();//makes sure that the current stats for that player are their maximum stats(for AI)


    PlayerTurn = (PlayerTurn + 1) % NumOfPlayers;

    Player * NewPlayer = Players[PlayerTurn];
    NewPlayer->AssertDommination();

    NewPlayer->Money += Players[PlayerTurn]->GetIncomeFromBuildings();

    NewPlayer->DoAITurn(Players);
}
void DeleteAllPlayers(){
    for (int p : range(Players.size()))
        DeletePlayer(0);
}
void DeletePlayer(int PNum){
    Players[PNum]->ClearData();
    if (Players[PNum]->Type == Real)
        delete (reinterpret_cast<RealPlayer *>(Players[PNum]));
    else
        delete (reinterpret_cast<ComputerPlayer *>(Players[PNum]));
    Players.erase(Players.begin() + PNum);
}
