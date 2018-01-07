#pragma once
#include "Globals.h"
//temporary hack fixes
enum Pictures {
	SoldierTxt, CatapultTxt,//Troops
	TorchTxt, ArmorTxt, HorseTxt, BATxt,//Attachments
	ArmoryTxt, StableTxt, WallTxt, BA_FactTxt, TorchFactTxt, CataFactTxt, WallIconTxt, SmallFarmTxt, LargeFarmTxt, TrainCenterTxt
};//Buildings
enum Color { Green, Red, Blue, Black, White, Yellow };
#ifdef GRAPHICSLIB_EXPORTS
#define Export __declspec(dllexport)
#else
#define Export __declspec(dllimport)
#endif


Export extern int XCorner, YCorner;
Export extern int XWINSIZE, YWINSIZE;
Export extern int BuildInterfaceHight;
Export extern HINSTANCE hInst;
Export void ConvertPixelToSquare(int XIn, int YIn, int & XOut, int & YOut);
Export INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
Export extern bool ShowPlayerAreas;
Export void SetHInst(HINSTANCE new_hinst);
Export bool BuildFactories();
Export bool ResetRenderTarget(HWND hWnd);
Export void ReleaseAllResources();
Export void GetBoardSizeForEverything(HWND hWnd);
Export void ConvertSquareToPixel(float InX, float InY, int & OutX, int & OutY);
Export void DrawBaseBoard();
Export void BeginDraw();
Export void EndDraw(HWND hWnd);
Export void DrawTroopPicture(Pictures Bitmap, float XSquare, float YSquare, bool Flip = false, double Angle = 0);
Export void DrawBuildPicture(Pictures Bitmap, int XPart, int YPart, int XSquare, int YSquare);
Export void DrawColorSquare(Color InColor, int xspot, int yspot, float Opacity = 1.0f);
Export void ShadeWithColor(Color InColor, int xpix1, int ypix1, int xpix2, int ypix2, float Opacity = 1.0f);
Export void DrawInterfacePicture(Pictures Bitmap, int XPix, int YPix);
#ifdef Debug_Macro_Move
Export void DrawMacroMoveStuff(vector<DArray2d<TroopInfo<Array2d<double>>>> InData[4], int Spots[4]);
Export void DrawMacroMoveBuild(DArray2d<Array2d<double>> & BData, int Spots[2]);
Export void DrawMicroMoveStuff(vector<vector<MoveSquareVals>> & MData, int Spots[4]);
Export void DrawMajorChoice(int Size, int Spot);
#endif
Export void DrawMoney(int Money);
Export void SetHWND(HWND InhWnd);
Export void UncondMakeDialogue(LPCWSTR Body, LPCWSTR Caption);
inline void MakeDialogue(LPCWSTR Body, LPCWSTR Caption) {
	if (IsRealPlayerTurn())
		UncondMakeDialogue(Body, Caption);
}
Export bool DemolishDialogue();
Export Attach::Attachment BuyerDialogue(TInfo::TroopTypes Type);
Export int NumOfSolidersDialogue();
Export int NumOfCatasDialogue();
Export void DrawBuildingInterface();
Export void DrawTurnInterface(int Miniturn);
Export void ShadeSelectedBuilding(SelectTypes Build);
Export Color GetPlayerColor(int PlayerNum);
#undef Export