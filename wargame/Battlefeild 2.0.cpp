// Battlefeild 2.0.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Graphics.h"
#include "Base.h"
#include "Interface.h"

#define MAX_LOADSTRING 100

// Global Variables:
//HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);


void LeftClick(int xpix,int ypix);
void RightClick(int xpix,int ypix);
void InitiateGameResources(HWND hWnd);
void ExecuteCharKey(int Id);
void ExecuteExtraKeys(int Id);
void ExecuteMenuItem(int Id,HWND hWnd);
void DrawEverything(HWND hWnd);

#ifdef Debug_Macro_Move
int Levels[MaxLevels] = { 0, 0, 0, 0, 0 };
#endif

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,_In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_BATTLEFEILD20, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BATTLEFEILD20));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BATTLEFEILD20));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_BATTLEFEILD20);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_CREATE:		
		InitiateGameResources(hWnd);
		break;
	case WM_LBUTTONUP:
		LeftClick(LOWORD(lParam),HIWORD(lParam));
		break;
	case WM_RBUTTONUP:
		RightClick(LOWORD(lParam),HIWORD(lParam));
		break;
	case WM_KEYDOWN:
		ExecuteExtraKeys(wParam);
		break;
	case WM_CHAR:
		ExecuteCharKey(wParam);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		ExecuteMenuItem(wmId,hWnd);
		break;
	case WM_SIZE:
		GetBoardSizeForEverything(hWnd);
		InvalidateRect(hWnd, NULL, FALSE);
        break;
	case WM_TIMER:
		InvalidateRect(hWnd, NULL, FALSE);	
		break;
	case WM_PAINT:
		DrawEverything(hWnd);
		break;
	case WM_DESTROY:
		SaveGameInfo(L"AutoSave.bin");
		DeleteAllPlayers();
		ReleaseAllResources();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


bool IsRealPlayerTurn(){
	return (Players[PlayerTurn]->Type == Real);
}
void ExecuteMenuItem(int Id,HWND hWnd){
	switch (Id)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_INSTRUCTIONS:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_INSTRUCTIONBOX), hWnd, About);
			break;
		case IDM_CATAPULTBOX:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_CATAPULTBOX), hWnd, About);
			break;
		case IDM_SOLDIERBOX:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_SOLDIERBOX), hWnd, About);
			break;
		case IDM_TORCHBOX:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_TORCHBOX), hWnd, About);
			break;
		case IDM_HORSEBOX:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_HORSEBOX), hWnd, About);
			break;
		case IDM_BA_BOX:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_BA_BOX), hWnd, About);
			break;
		case IDM_ARMORBOX:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ARMORBOX), hWnd, About);
			break;
		case IDM_SFARMBOX:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_SFARMBOX), hWnd, About);
			break;
		case IDM_LFARMBOX:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_LFARMBOX), hWnd, About);
			break;
		case IDM_TCENTERBOX:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_TCENTERBOX), hWnd, About);
			break;
		case IDM_CATFACTORYBOX:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_CATFACTORYBOX), hWnd, About);
			break;
		case IDM_TORCHFACTBOX:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_TORCHFACTBOX), hWnd, About);
			break;
		case IDM_BA_FACTORYBOX:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_BA_FACTORYBOX), hWnd, About);
			break;
		case IDM_ARMORYBOX:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ARMORYBOX), hWnd, About);
			break;
		case IDM_STABLEBOX:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_STABLEBOX), hWnd, About);
			break;
		case IDM_WALLBOX:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_WALLBOX), hWnd, About);
			break;
		case IDM_SHOWPLAYERAREAS:
			ShowPlayerAreas = true;
			break;
		case IDM_HIDEPLAYERAREAS:
			ShowPlayerAreas = false;
			break;
		case IDM_SAVESLOT1:
			SaveGameInfo(L"SaveSlot1.bin");
			break;
		case IDM_SAVESLOT2:
			SaveGameInfo(L"SaveSlot2.bin");
			break;
		case IDM_SAVESLOT3:
			SaveGameInfo(L"SaveSlot3.bin");
			break;
		case IDM_LOADAUTOSAVE:
			LoadGameInfo(L"AutoSave.bin");
			break;
		case IDM_LOADSLOT1:
			SaveGameInfo(L"AutoSave.bin");
			LoadGameInfo(L"SaveSlot1.bin");
			break;
		case IDM_LOADSLOT2:
			SaveGameInfo(L"AutoSave.bin");
			LoadGameInfo(L"SaveSlot2.bin");
			break;
		case IDM_LOADSLOT3:
			SaveGameInfo(L"AutoSave.bin");
			LoadGameInfo(L"SaveSlot3.bin");
			break;
		case IDM_CLEARGAME:
			SaveGameInfo(L"AutoSave.bin");
			ResetGame();
			break;
	}
}
void ExecuteExtraKeys(int Id){
	switch(Id){
		case VK_RIGHT:
			if(XCorner < BoardSizeX - 1)
				XCorner += 1;
			break;
		case VK_LEFT:
			if(XCorner > -BoardSizeX + 1)
				XCorner -= 1;
			break;
		case VK_UP:
			if(YCorner > -BoardSizeY + 1)
				YCorner -= 1;
			break;
		case VK_DOWN:
			if(YCorner < BoardSizeY - 1)
				YCorner += 1;
			break;
		case VK_ADD:
			if(BuildInterfaceHight + 1 < NumOfBuildingTypes)
				BuildInterfaceHight += 1;
			break;
		case VK_SUBTRACT:
			if(BuildInterfaceHight > 0)
				BuildInterfaceHight -= 1;
			break;
		}
}
void ExecuteCharKey(int Id){
	Player * Play = Players[PlayerTurn];
	switch(Id){
	case 's':
		Play->SelectThing(SFarmSelect);
		break;
	case 'l':
		Play->SelectThing(LFarmSelect);
		break;
	case 't':
		Play->SelectThing(TrainCenterSelect);
		break;
	case 'c':
		Play->SelectThing(CataFactSelect);
		break;
	case 'o':
		Play->SelectThing(TorchFactorySelect);
		break;
	case 'b':
		Play->SelectThing(BA_FactorySelect);
		break;
	case 'a':
		Play->SelectThing(ArmorySelect);
		break;
	case 'h':
		Play->SelectThing(StableSelect);
		break;
	case 'w':
		Play->SelectThing(WallSelect);
		break;
	}
}
void InitiateGameResources(HWND hWnd){
	PlayerTurn = 0;
	srand(reinterpret_cast<unsigned int>(hWnd));//acts as a random enough seed
	Players.resize(2);
	Players[0] = reinterpret_cast<Player *> (new RealPlayer);
	Players[0]->PlayerNum = 0;
	Players[0]->TeamNum = 0;
	Players[1] = reinterpret_cast<Player *> (new ComputerPlayer);
	Players[1]->PlayerNum = 1;
	Players[1]->TeamNum = 1;
	ResetGame();
	SetHWND(hWnd);
	if(BuildFactories() == false)
		::MessageBox(hWnd,_T("Epic fail with factory"),NULL,IDOK);
	//also loads bitmaps
	if(ResetRenderTarget(hWnd) == false)
		::MessageBox(hWnd,_T("Epic fail with render target creation or brushes"),NULL,IDOK);
	//timer, very important
	double FPS = 20.0;
#ifdef DrawMacroPath
	FPS = 0.3;
#endif
#ifdef ShowSubtractEffect
	FPS = 0.2;
#endif
#ifdef DrawRandomPath
	FPS = 0.6666;
#endif
#ifdef DrawMoveValLoc
	FPS = 0.2;
#endif
	SetTimer(hWnd,NULL,1000/FPS,NULL);
}
void DrawPlayerAreas(){
	for (Point P : BoardIterate()){
		if(PlayerDom[P].Influence >= 0){
			DrawColorSquare(GetPlayerColor(PlayerDom[P].Player),P.X,P.Y,(PlayerDom[P].Influence * 0.5f) / MaximumDomination);
		}
	}
}
void DrawEverything(HWND hWnd){
	BeginDraw();//must call BeginDraw before any drawing functions

	DrawBaseBoard();
	DrawBuildingInterface();
	
	for(Player * P : Players)
		P->Draw();

	if(ShowPlayerAreas)
		DrawPlayerAreas();

	DrawTurnInterface(Players[PlayerTurn]->MiniTurn);
	Players[PlayerTurn]->DrawInterface();

	EndDraw(hWnd);//must call EndDraw before breaking
} 
void LeftClick(int xpix,int ypix){
#ifdef Debug_Macro_Move
	if (xpix > XWINSIZE - DEBUG_MACRO_WIDTH * MaxLevels){
		Levels[(XWINSIZE - xpix) / DEBUG_MACRO_WIDTH] = ypix / DEBUG_MACRO_HEIGHT;
		
		return;
	}
#endif
	int xbox,ybox;
	ConvertPixelToSquare(xpix,ypix,xbox,ybox);
	if(xbox >= 0 && ybox >= 0)
		Players[PlayerTurn]->LeftClick(xbox,ybox);
	else if(xpix < XINTERFACE){
		if((YWINSIZE - ypix) / TURNBOXCOEF < 3){
			if(Players[PlayerTurn]->ChangeTurn()){
				ChangeOverallTurn();
			}
		}
		else if(ypix < BUILDINTERFACECOEF * (NumOfBuildingTypes - BuildInterfaceHight) && Players[PlayerTurn]->MiniTurn == 0)
			Players[PlayerTurn]->SelectThing(static_cast<SelectTypes>(ypix / BUILDINTERFACECOEF + BuildInterfaceHight));
	}
}
void RightClick(int xpix,int ypix){
	int xbox,ybox;
	ConvertPixelToSquare(xpix,ypix,xbox,ybox);
	if(xbox >= 0 && ybox >= 0)
		Players[PlayerTurn]->RightClick(xbox,ybox);
}