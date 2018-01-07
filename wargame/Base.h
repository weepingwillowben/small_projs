#include "stdafx.h"
#include "RealPlayer.h"
#include "ComputerPlayer.h"

extern int PlayerTurn;
extern vector<Player *> Players;
extern int SRandVal;
void SaveGameInfo(LPCWSTR FileName);
bool LoadGameInfo(LPCWSTR FileName);
void ResetGame();
void DeletePlayer(int PNum);
void DeleteAllPlayers();
