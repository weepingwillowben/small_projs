#include "stdafx.h"
#include "Graphics.h"
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#pragma comment(lib, "dwrite.lib")
#include <wincodec.h>
#pragma comment(lib, "WindowsCodecs.lib")
#pragma comment(lib, "d2d1")
template<class Interface>
inline void SafeRelease(Interface **ppInterfaceToRelease)
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();

        (*ppInterfaceToRelease) = NULL;
    }
}
//direct 2d bitmaps and more
ID2D1SolidColorBrush* pBlackBrush;
ID2D1SolidColorBrush* pGreenBrush;
ID2D1SolidColorBrush* pYellowBrush;
ID2D1SolidColorBrush* pRedBrush;
ID2D1SolidColorBrush* pBlueBrush;
ID2D1SolidColorBrush* pWhiteMaskBrush;
ID2D1SolidColorBrush* pLightBlueBrush;
ID2D1SolidColorBrush* pLightRedBrush;
ID2D1SolidColorBrush* pLightGreenBrush;

ID2D1BitmapBrush* SoldierBitmap;

ID2D1BitmapBrush* SoldierArmorBitmap;
ID2D1BitmapBrush* SoldierTorchBitmap;
ID2D1BitmapBrush* SoldierHorseBitmap;
ID2D1BitmapBrush* SoldierBABitmap;

ID2D1BitmapBrush* CatapultBitmap;

ID2D1BitmapBrush* SFarmBitmap;
ID2D1BitmapBrush* LFarmBitmap;
ID2D1BitmapBrush* CatFactBitmap;
ID2D1BitmapBrush* TCenterBitmap;
ID2D1BitmapBrush* BA_FactBitmap;
ID2D1BitmapBrush* TorchFactBitmap;
ID2D1BitmapBrush* ArmoryBitmap;
ID2D1BitmapBrush* StableBitmap;
ID2D1BitmapBrush* WallBitmap;

ID2D1BitmapBrush* WallIconBitmap;
ID2D1BitmapBrush* FireBitmap;
ID2D1BitmapBrush* DestructionBitmap;

ID2D1BitmapBrush* BackgroundTexture;

IDWriteTextFormat* pMoneyForm;
IDWriteTextFormat* pMoveForm;
IDWriteTextFormat* pAttackForm;
IDWriteTextFormat* pBuildForm;

IWICImagingFactory *pIWICFactory;

ID2D1Factory* pD2DFactory;

IDWriteFactory * pWriteFact;

ID2D1HwndRenderTarget* pRT;

HWND hWnd;
#include "Interface.h"

int XCorner = 0, YCorner = 0;
int XWINSIZE, YWINSIZE;
bool ShowPlayerAreas = false;
int BuildInterfaceHight = 0;
HINSTANCE hInst;
bool LoadAllBitmaps();
HRESULT CreateBrushes();
//basic necessary graphics functions
void SetHInst(HINSTANCE new_hinst) {
	hInst = new_hinst;
}
bool BuildFactories(){
    HRESULT FactResult = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
    if(SUCCEEDED(FactResult)){
        FactResult = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&pWriteFact)
            );
    }
    if(SUCCEEDED(FactResult)){
        FactResult = ::CoInitialize(NULL);

        FactResult = ::CoCreateInstance(
            CLSID_WICImagingFactory, // Has to be replaced with CLSID_WICImagingFactory1
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_IWICImagingFactory,
            (LPVOID*) (&pIWICFactory)
            );
    }
    if(SUCCEEDED(FactResult))
        return true;
    else
        return false;
}
bool ResetRenderTarget(HWND hWnd){
    // Obtain the size of the drawing area.
    SafeRelease(&pRT);

    HRESULT hr;
    RECT rc;
    GetClientRect(hWnd, &rc);
    // Create a Direct2D render target
    pRT = NULL;
    hr = pD2DFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(rc.right - rc.left,rc.bottom - rc.top)),
        &pRT
    );
    if(SUCCEEDED(hr)){//loads bitmaps, vital and first
        if(LoadAllBitmaps() == false)
            return false;
    }
    if(SUCCEEDED(hr)){//loads bitmaps, vital and first
        hr = CreateBrushes();
    }
    //create all the class specific resources here
    if(SUCCEEDED(hr)){
        return true;
    }
    else{
        ::MessageBox(hWnd,_T("Error creating target or brushes"),NULL,IDOK);
        return false;
    }
}
void GetBoardSizeForEverything(HWND hWnd){
    RECT rc;
    GetClientRect(hWnd, &rc);
    pRT->Resize(D2D1::SizeU( rc.right - rc.left,rc.bottom - rc.top));
    XWINSIZE = rc.right - rc.left;
    YWINSIZE = rc.bottom - rc.top;
}
void ReleaseAllResources(){
    SafeRelease(&SoldierBitmap);

    SafeRelease(&CatapultBitmap);

    SafeRelease(&SFarmBitmap);
    SafeRelease(&LFarmBitmap);
    SafeRelease(&CatFactBitmap);
    SafeRelease(&TCenterBitmap);
    SafeRelease(&BA_FactBitmap);
    SafeRelease(&TorchFactBitmap);
    SafeRelease(&ArmoryBitmap);
    SafeRelease(&StableBitmap);
    SafeRelease(&WallBitmap);

    SafeRelease(&WallIconBitmap);

    SafeRelease(&FireBitmap);
    SafeRelease(&DestructionBitmap);

    SafeRelease(&pIWICFactory);

    SafeRelease(&pWriteFact);

    SafeRelease(&pRT);

    SafeRelease(&pD2DFactory);

    SafeRelease(&pBlackBrush);
    SafeRelease(&pGreenBrush);
    SafeRelease(&pRedBrush);
    SafeRelease(&pBlueBrush);
    SafeRelease(&pWhiteMaskBrush);
    SafeRelease(&pLightBlueBrush);
    SafeRelease(&pLightRedBrush);
    SafeRelease(&pLightGreenBrush);

    SafeRelease(&pMoneyForm);
    SafeRelease(&pMoveForm);
    SafeRelease(&pAttackForm);
    SafeRelease(&pBuildForm);
}
bool LoadBitmapBrushFromFile(LPCWSTR FileName,ID2D1BitmapBrush** ppBitmapBrush){
    ID2D1Bitmap * pBitmap = NULL;
    ID2D1Bitmap ** ppBitmap = &pBitmap;
    HRESULT hr;

    IWICBitmapDecoder *pDecoder = NULL;
    IWICBitmapFrameDecode *pSource = NULL;
    IWICStream *pStream = NULL;
    IWICFormatConverter *pConverter = NULL;
    IWICBitmapScaler *pScaler = NULL;

    hr = pIWICFactory->CreateDecoderFromFilename(
        FileName,
        NULL,
        GENERIC_READ,
        WICDecodeMetadataCacheOnDemand,
        &pDecoder
        );
    if (SUCCEEDED(hr))
    {
        // Create the initial frame.
        hr = pDecoder->GetFrame(0, &pSource);
    }
    if (SUCCEEDED(hr))
    {
        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
        hr = pIWICFactory->CreateFormatConverter(&pConverter);
    }
    if (SUCCEEDED(hr))
    {
        hr = pConverter->Initialize(
           pSource,
           GUID_WICPixelFormat32bppPBGRA,
           WICBitmapDitherTypeNone,
           NULL,
           0.f,
           WICBitmapPaletteTypeMedianCut
           );
    }
    if (SUCCEEDED(hr))
    {
        hr = pRT->CreateBitmapFromWicBitmap(pConverter, NULL, ppBitmap);
    }
    if (SUCCEEDED(hr)){
        hr = pRT->CreateBitmapBrush(
            (*ppBitmap),
            ppBitmapBrush);
    }

    SafeRelease(&pDecoder);
    SafeRelease(&pSource);
    SafeRelease(&pStream);
    SafeRelease(&pConverter);
    SafeRelease(&pScaler);
    SafeRelease(ppBitmap);

    if(SUCCEEDED(hr))
        return true;
    else
        return false;
    }
bool LoadAllBitmaps(){
    if(LoadBitmapBrushFromFile(_T("Pictures/Soldier.png"),&SoldierBitmap) == false)
        return false;
    if(LoadBitmapBrushFromFile(_T("Pictures/Armor.png"),&SoldierArmorBitmap) == false)
        return false;
    if(LoadBitmapBrushFromFile(_T("Pictures/Horse.png"),&SoldierHorseBitmap) == false)
        return false;
    if(LoadBitmapBrushFromFile(_T("Pictures/BA.png"),&SoldierBABitmap) == false)
        return false;
    if(LoadBitmapBrushFromFile(_T("Pictures/Torch.png"),&SoldierTorchBitmap) == false)
        return false;
    if(LoadBitmapBrushFromFile(_T("Pictures/Catapult.png"),&CatapultBitmap) == false)
        return false;
    if(LoadBitmapBrushFromFile(_T("Pictures/SmallFarm.bmp"),&SFarmBitmap) == false)
        return false;
    if(LoadBitmapBrushFromFile(_T("Pictures/LargeFarm.bmp"),&LFarmBitmap) == false)
        return false;
    if(LoadBitmapBrushFromFile(_T("Pictures/CatFactory.bmp"),&CatFactBitmap) == false)
        return false;
    if(LoadBitmapBrushFromFile(_T("Pictures/TrainCenter.bmp"),&TCenterBitmap) == false)
        return false;
    if(LoadBitmapBrushFromFile(_T("Pictures/BA_Factory.bmp"),&BA_FactBitmap) == false)
        return false;
    if(LoadBitmapBrushFromFile(_T("Pictures/TorchFactory.bmp"),&TorchFactBitmap) == false)
        return false;
    if(LoadBitmapBrushFromFile(_T("Pictures/Armory.bmp"),&ArmoryBitmap) == false)
        return false;
    if(LoadBitmapBrushFromFile(_T("Pictures/Stable.bmp"),&StableBitmap) == false)
        return false;
    if(LoadBitmapBrushFromFile(_T("Pictures/WallMiddle.bmp"),&WallBitmap) == false)
        return false;
    if(LoadBitmapBrushFromFile(_T("Pictures/WallIcon.bmp"),&WallIconBitmap) == false)
        return false;
    if(LoadBitmapBrushFromFile(_T("Pictures/FireSquare.bmp"),&FireBitmap) == false)
        return false;
    if(LoadBitmapBrushFromFile(_T("Pictures/DestroyedSquare.bmp"),&DestructionBitmap) == false)
        return false;
    if(LoadBitmapBrushFromFile(_T("Pictures/Background.bmp"),&BackgroundTexture) == false)
        return false;

    return true;
}
HRESULT CreateBrushes(){
    HRESULT hr = S_OK;

        if (SUCCEEDED(hr)){
            // Create a black brush.
            hr = pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black),&pBlackBrush);
        }
        if (SUCCEEDED(hr)){
            // Create a Blue brush.
            hr = pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue),&pBlueBrush);
        }
        if (SUCCEEDED(hr)){
            // Create a opaque, light blue brush.
            hr = pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightBlue),D2D1::BrushProperties(0.5),&pLightBlueBrush);
        }
        if (SUCCEEDED(hr)){
            // Create a opaque, light red brush.
            hr = pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red),D2D1::BrushProperties(0.5),&pLightRedBrush);
        }
        if (SUCCEEDED(hr)){
            // Create a opaque, light green brush.
            hr = pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::GreenYellow),D2D1::BrushProperties(0.5),&pLightGreenBrush);
        }
        if (SUCCEEDED(hr)){
            // Create a opaque, white brush.
            hr = pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White),D2D1::BrushProperties(0.5),&pWhiteMaskBrush);
        }
        if (SUCCEEDED(hr)){
            // Create a green brush.
            hr = pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::GreenYellow),&pGreenBrush);
        }
        if (SUCCEEDED(hr)){
            // Create a red brush.
            hr = pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red),&pRedBrush);
        }
        if (SUCCEEDED(hr)){
            // Create a red brush.
            hr = pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow),&pYellowBrush);
        }
    if (SUCCEEDED(hr)){
        pWriteFact->CreateTextFormat(
            L"Bodoni MT",                // Font family name.
            NULL,                       // Font collection (NUL sets it to use the system font collection).
            DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            BLOCKSIZE * 0.6666f,
            L"en-us",
            &pMoneyForm);
    }
    if (SUCCEEDED(hr)){
        pWriteFact->CreateTextFormat(
            L"Algerian",                // Font family name.
            NULL,                       // Font collection (NUL sets it to use the system font collection).
            DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            BLOCKSIZE * 0.5f,
            L"en-us",
            &pAttackForm);
    }
    if (SUCCEEDED(hr)){
        pWriteFact->CreateTextFormat(
            L"Bauhaus 93",                // Font family name.
            NULL,                       // Font collection (NUL sets it to use the system font collection).
            DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            BLOCKSIZE * 0.6666f,
            L"en-us",
            &pMoveForm);
    }
    if (SUCCEEDED(hr)){
        pWriteFact->CreateTextFormat(
            L"Arial Rounded MT Bold",                // Font family name.
            NULL,                       // Font collection (NUL sets it to use the system font collection).
            DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            BLOCKSIZE * 0.6666f,
            L"en-us",
            &pBuildForm);
    }
    return hr;
}

//Graphics helper functions
bool IsOnScreen(int xbox,int ybox){
    return xbox >= XCorner && ybox >= YCorner;
}
void ConvertSquareToPixel(float InX,float InY,int & OutX,int & OutY){
    OutX = (InX - XCorner) * BLOCKSIZE + XINTERFACE;
    OutY = (InY - YCorner) * BLOCKSIZE;
}
void ConvertPixelToSquare(int XIn, int YIn, int & XOut,int & YOut){
    int PosXbox = (XIn - XINTERFACE) / BLOCKSIZE + XCorner;

    if(XIn - XINTERFACE > 0 && PosXbox < BoardSizeX)
        XOut = (XIn - XINTERFACE) / BLOCKSIZE + XCorner;
    else
        XOut = -1;

    int PosYbox = YIn / BLOCKSIZE + YCorner;
    if(PosYbox < BoardSizeY)
        YOut = YIn / BLOCKSIZE + YCorner;
    else
        YOut = -1;
}
void DrawTexture(ID2D1BitmapBrush * pBitmap,int XSize, int YSize,int XPix,int YPix){

    if(XPix < -XSize * BLOCKSIZE || XPix > XWINSIZE || YPix < - YSize * BLOCKSIZE || YPix > YWINSIZE)//if it is not on the screen
        return;

    pBitmap->SetTransform(D2D1::Matrix3x2F::Translation(XPix,YPix));
    pRT->FillRectangle(D2D1::RectF(XPix,YPix,XPix + XSize * BLOCKSIZE,YPix + YSize * BLOCKSIZE),pBitmap);
    pBitmap->SetTransform(D2D1::Matrix3x2F::Identity());
}
void DrawTexturePart(ID2D1BitmapBrush * pBitmap,int XPart,int YPart,int xpix,int ypix){

    if(xpix < -(1 + XPart) * BLOCKSIZE || xpix > XWINSIZE || ypix < -(1 + YPart) * BLOCKSIZE || ypix > YWINSIZE)//if it is not on the screen
        return;
    pBitmap->SetTransform(D2D1::Matrix3x2F::Translation(xpix,ypix));
    pRT->FillRectangle(D2D1::RectF(xpix + XPart * BLOCKSIZE,ypix + YPart * BLOCKSIZE,
                                    xpix + (1 + XPart) * BLOCKSIZE,ypix + (1 + YPart) * BLOCKSIZE),pBitmap);
    pBitmap->SetTransform(D2D1::Matrix3x2F::Identity());
}
void ShadeWithColor(Color InColor,int xpix1,int ypix1,int xpix2,int ypix2,float Opacity){
    ID2D1SolidColorBrush * pBrush;
    switch(InColor){
    case Green:pBrush = pGreenBrush;break;
    case Yellow:pBrush = pYellowBrush;break;
    case Red:pBrush = pRedBrush;break;
    case Blue: pBrush = pBlueBrush;break;
    case Black:pBrush = pBlackBrush;break;
    case White:pBrush = pWhiteMaskBrush;break;
    default:pBrush = pBlackBrush;
    }
    pBrush->SetOpacity(Opacity);
    pRT->FillRectangle(D2D1::RectF(xpix1,ypix1,xpix2 ,ypix2),pBrush);
    pBrush->SetOpacity(1.0f);
}
void DrawColorSquare(Color InColor,int xspot,int yspot,float Opacity){
    if(!IsOnScreen(xspot,yspot))
        return;
    int xpix,ypix;
    ConvertSquareToPixel(xspot,yspot,xpix,ypix);
    ShadeWithColor(InColor,xpix,ypix,xpix + BLOCKSIZE,ypix + BLOCKSIZE,Opacity);
}
ID2D1BitmapBrush * PicToBit(Pictures Bitmap){
    ID2D1BitmapBrush * pBitmap;

    switch(Bitmap){
    case SoldierTxt:pBitmap = SoldierBitmap; break;
    case TorchTxt:pBitmap = SoldierTorchBitmap; break;
    case ArmorTxt:pBitmap = SoldierArmorBitmap; break;
    case HorseTxt:pBitmap = SoldierHorseBitmap; break;
    case BATxt:pBitmap = SoldierBABitmap; break;
    case CatapultTxt:pBitmap = CatapultBitmap; break;
    case ArmoryTxt: pBitmap = ArmoryBitmap; break;
    case StableTxt: pBitmap = StableBitmap; break;
    case WallTxt: pBitmap = WallBitmap; break;
    case BA_FactTxt: pBitmap = BA_FactBitmap; break;
    case TorchFactTxt: pBitmap = TorchFactBitmap; break;
    case CataFactTxt: pBitmap = CatFactBitmap; break;
    case WallIconTxt: pBitmap = WallIconBitmap; break;
    case SmallFarmTxt: pBitmap = SFarmBitmap; break;
    case LargeFarmTxt: pBitmap = LFarmBitmap; break;
    case TrainCenterTxt: pBitmap = TCenterBitmap; break;
    default: pBitmap = NULL;
    };
    return pBitmap;
}
//draws the board stuff
void BeginDraw(){
    pRT->BeginDraw();

    pRT->SetTransform(D2D1::Matrix3x2F::Identity());

    pRT->Clear(D2D1::ColorF(D2D1::ColorF::White));
}
void EndDraw(HWND hWnd){
    HRESULT hr;
    hr = pRT->EndDraw();

    if(!SUCCEEDED(hr))
        ResetRenderTarget(hWnd);

    ValidateRect(hWnd, NULL);
}
void DrawBaseBoard(){
    for(int x : range(max(XCorner,0),BoardSizeX)){
        for(int y : range(max(YCorner,0),BoardSizeY)){
            int xpix,ypix;
            ConvertSquareToPixel(x,y,xpix,ypix);
            DrawTexture(BackgroundTexture,1,1,xpix,ypix);
#ifdef ShowSquareNums
            auto Rect = D2D1::RectF(xpix, ypix, xpix + 2 *BLOCKSIZE, ypix + BLOCKSIZE);
            wstring str = (to_wstring(x%10) + L"," + to_wstring(y%10));
            pRT->DrawText(str.c_str(),str.size(),pMoneyForm,Rect,pBlackBrush);
#endif
        }
    }
}
void DrawTroopPicture(Pictures Bitmap,float XSquare,float YSquare,bool Flip,double Angle){
    ID2D1BitmapBrush * pBitmap = PicToBit(Bitmap);
    //implement angle and flip here
    int xpix,ypix;
    ConvertSquareToPixel(XSquare,YSquare,xpix,ypix);

    if(xpix < -BLOCKSIZE || xpix > XWINSIZE || ypix < - BLOCKSIZE || ypix > YWINSIZE)//if it is not on the screen
        return;
    D2D1::Matrix3x2F BitMatrix = D2D1::Matrix3x2F::Identity();
    D2D1::Matrix3x2F BackMatrix = D2D1::Matrix3x2F::Identity();
    if(Flip){
        D2D1::Matrix3x2F FlipMatrix;
        //sets the flipmatrix manually
        FlipMatrix._11 = -1;
        FlipMatrix._12 = 0;
        FlipMatrix._21 = 0;
        FlipMatrix._22 = 1;
        FlipMatrix._31 = 0;
        FlipMatrix._32 = 0;
        BitMatrix = BitMatrix * FlipMatrix;
    }
    if(Angle != 0)
        BackMatrix = D2D1::Matrix3x2F::Rotation(Angle,D2D1::Point2F((0.5f - Flip) * BLOCKSIZE,0.5f * BLOCKSIZE)) * BackMatrix;
    BackMatrix = BackMatrix * D2D1::Matrix3x2F::Translation(xpix + Flip * BLOCKSIZE,ypix);
    pBitmap->SetTransform(BitMatrix);
    pRT->SetTransform(BackMatrix);
    pRT->FillRectangle(D2D1::RectF(-Flip * BLOCKSIZE,0,(1 - Flip) * BLOCKSIZE,BLOCKSIZE),pBitmap);//this nonsense wis necessary for flipping
    pBitmap->SetTransform(D2D1::Matrix3x2F::Identity());
    pRT->SetTransform(D2D1::Matrix3x2F::Identity());
}
void DrawBuildPicture(Pictures Bitmap,int XPart,int YPart,int XSquare,int YSquare){
    if(XSquare + XPart < XCorner)
        return;
    ID2D1BitmapBrush * pBitmap = PicToBit(Bitmap);

    int xpix,ypix;
    ConvertSquareToPixel(XSquare,YSquare,xpix,ypix);
    DrawTexturePart(pBitmap, XPart, YPart, xpix, ypix);
}
void DrawInterfacePicture(Pictures Bitmap,int XPix,int YPix){
    ID2D1BitmapBrush * pBitmap = PicToBit(Bitmap);

    DrawTexture(pBitmap,2,2,XPix,YPix);
}
const wchar_t * Str_To_CWC(string & S,wstring & Buff){
    Buff = wstring(S.begin(),S.end());
    return Buff.c_str();
}
void DrawMoney(int Money){
    //draws the box:

    D2D1_RECT_F Rect = D2D1::RectF(0,YWINSIZE - TURNBOXCOEF * 3 - BLOCKSIZE,2*BLOCKSIZE,YWINSIZE - TURNBOXCOEF * 3);

    pWhiteMaskBrush->SetOpacity(1.0f);
    pRT->FillRectangle(Rect,pWhiteMaskBrush);//background
    Rect.bottom -= 1;
    Rect.top += 1;
    Rect.left += 1;
    Rect.right -= 1;

    pRT->DrawRectangle(Rect,pBlackBrush,BLOCKSIZE / 20);

    Rect.left = Rect.left + 2;

    wstring Buffer;
    string DrawStr = string("$") + to_string(Money);
    pRT->DrawText(Str_To_CWC(DrawStr,Buffer),DrawStr.size(),pMoneyForm,Rect,pBlackBrush);
}
void SetHWND(HWND InhWnd){
    hWnd = InhWnd;
}
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
INT_PTR CALLBACK SOLDIER(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:

        if (LOWORD(wParam) == IDCANCEL || LOWORD(wParam) >= 114 && LOWORD(wParam) <= 117)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
INT_PTR CALLBACK SoldierBuyer(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        // cancels and returns value
        if (LOWORD(wParam) == IDCANCEL || LOWORD(wParam) >= 154 && LOWORD(wParam) <= 163)
        {// minuses that much so it returns a value from 1-10
            if(LOWORD(wParam) == IDCANCEL)
                EndDialog(hDlg, 0);
            else
                EndDialog(hDlg, LOWORD(wParam) - 153);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
void UncondMakeDialogue(LPCWSTR Body,LPCWSTR Caption){
    ::MessageBox(hWnd, Body, Caption, IDOK);
}
bool DemolishDialogue(){
    int ReturnVal = DialogBox(hInst, MAKEINTRESOURCE(IDD_BULLDOZER), hWnd, About);
    if(ReturnVal == 1)
        return true;
    else
        return false;
}
Attach::Attachment BuyerDialogue(TInfo::TroopTypes Type){
    using namespace Attach;
    if(Type == TInfo::SoldierType){
        int ReturnVal = DialogBox(hInst, MAKEINTRESOURCE(IDD_SOLDIERSELECTION), hWnd, SOLDIER);
        switch(ReturnVal){
        case IDBUYHORSE: return Horse;
        case IDBUYARMOR: return Armor;
        case IDBUYTORCH: return Torch;
        case IDBUYBA: return BA;
        }
    }
    else if(Type == TInfo::CatapultType){
        int ReturnVal = DialogBox(hInst, MAKEINTRESOURCE(IDD_CATAPULTSELECTION), hWnd, SOLDIER);
        switch(ReturnVal){
        case IDBUYHORSE: return CataHorse;
        }
    }
    return NoAttach;
}
#ifdef Debug_Macro_Move
void DrawDebugText(string S,int yloc){
    wstring W;
    int Right = XWINSIZE - DEBUG_MACRO_WIDTH * 4;
    D2D1_RECT_F Rect = D2D1::RectF(Right - 200, yloc*20, Right, (yloc+1)*20);
    pRT->DrawTextW(Str_To_CWC(S, W), S.size(),pMoneyForm,Rect,pBlackBrush);
}
void DrawDebugData(Array2d<double> & Data){
    double MaxD = 0;
    for (double Info : Data)
        MaxD = max(MaxD, abs(Info));

    for (Point P : BoardIterate()){
        double D = Data[P];
        Color C = D > 0 ? Blue : Black;
        DrawColorSquare(C, P.X, P.Y, abs(D) / MaxD);
    }
    DrawDebugText(to_string(MaxD),0);
}
void DrawPoint(int x, int y,Color C){
    int xpix = XWINSIZE - (x+1) * DEBUG_MACRO_WIDTH;
    int ypix = y * DEBUG_MACRO_HEIGHT;
    ShadeWithColor(C, xpix+1, ypix+1, xpix + DEBUG_MACRO_WIDTH-1, ypix + DEBUG_MACRO_HEIGHT-1);
}
void DrawPoints(int Sizes[],int Spots[],int Size){
    for (int x : range(Size)){
        for (int y : range(Sizes[x])){
            DrawPoint(x+1, y, Yellow);
        }
    }
    for (int x : range(Size)){
        int y = Spots[x];
        DrawPoint(x+1, y, Red);
    }
}
bool IsInScope(int Sizes[], int Spots[], int Size){
    for (int n : range(Size))
        if (Spots[n] < 0 || Spots[n] >= Sizes[n])
            return false;
    return true;
}
void DrawMacroMoveStuff(vector<DArray2d<TroopInfo<Array2d<double>>>> InData[4], int Spots[4]){
    if (InData[0].size() <= 0 || InData[0][0].dim1() <= 0 || InData[0][0][0].Size < 0)
        return;
    int Sizes[4];
    Sizes[0] = 4;
    Sizes[1] = InData[0].size();
    Sizes[2] = InData[0][0].dim1();
    Sizes[3] = InData[0][0][0].Size;

    if (IsInScope(Sizes,Spots,4))
        DrawDebugData(InData[Spots[0]][Spots[1]][Spots[2]][Spots[3]].Info);

    DrawPoints(Sizes,Spots,4);
}
void DrawMacroMoveBuild(DArray2d<Array2d<double>> & BData, int Spots[2]){
    if (BData.Data.size() == 0)
        return;
    int Sizes[2] = { BData.dim1(), BData.dim2() };

    if (IsInScope(Sizes,Spots,2)){
        DrawDebugData(BData[Spots[0]][Spots[1]]);
    }
    DrawPoints(Sizes,Spots,2);
}
void DrawMicroMoveStuff(vector<vector<MoveSquareVals>> & MData, int Spots[4]){
    if (MData.size() == 0 || MData[0].size() == 0 || MData[0][0].Size() == 0)
        return;
    const int Size = 3;
    int Sizes[Size] = { MData.size(), MData[0].size(), MData[0][0].Size() + 3};
    if (IsInScope(Sizes, Spots, Size)){
        Array2d<double> Vals(0);
        if (Spots[2] == Sizes[2] - 1) {
            for (auto MPair : MData[Spots[0]][Spots[1]])
                Vals[MPair.P] = MPair.Info().MoveV + max(MPair.Info().AttackV.Arr);
        }
        else if (Spots[2] == Sizes[2] - 2){
            for (auto MPair : MData[Spots[0]][Spots[1]])
                Vals[MPair.P] = max(MPair.Info().AttackV.Arr);
        }
        else if (Spots[2] == Sizes[2] - 3) {
            for (auto MPair : MData[Spots[0]][Spots[1]])
                Vals[MPair.P] = MPair.Info().MoveV;
        }
        else{
            for (auto APair : MData[Spots[0]][Spots[1]].Arr[Spots[2]].AttackV)
                Vals[APair.P] = APair.Info();
        }
        DrawDebugData(Vals);
    }
    DrawPoints(Sizes, Spots, Size);
}
void DrawMajorChoice(int Size, int Spot){
    for (int s : range(Size)){
        DrawPoint(0, s, Yellow);
    }
    if (Spot < NumOfMChoices)
        DrawPoint(0, Spot, Red);
}
#endif
int NumOfSolidersDialogue(){
    return DialogBox(hInst, MAKEINTRESOURCE(IDD_SOLDIERBUYER), hWnd, SoldierBuyer);
}
int NumOfCatasDialogue(){
    return DialogBox(hInst, MAKEINTRESOURCE(IDD_CATAPULTBUYER), hWnd, SoldierBuyer);
}
void DrawBuildingInterface(){
    int YPix = -BuildInterfaceHight * BUILDINTERFACECOEF;
    DrawInterfacePicture(SmallFarmTxt,0,YPix);
    YPix += BUILDINTERFACECOEF;
    DrawInterfacePicture(LargeFarmTxt,0,YPix);
    YPix += BUILDINTERFACECOEF;
    DrawInterfacePicture(CataFactTxt,0,YPix);
    YPix += BUILDINTERFACECOEF;
    DrawInterfacePicture(TrainCenterTxt,0,YPix);
    YPix += BUILDINTERFACECOEF;
    DrawInterfacePicture(BA_FactTxt,0,YPix);
    YPix += BUILDINTERFACECOEF;
    DrawInterfacePicture(TorchFactTxt,0,YPix);
    YPix += BUILDINTERFACECOEF;
    DrawInterfacePicture(ArmoryTxt,0,YPix);
    YPix += BUILDINTERFACECOEF;
    DrawInterfacePicture(StableTxt,0,YPix);
    YPix += BUILDINTERFACECOEF;
    DrawInterfacePicture(WallIconTxt,0,YPix);
}
void DrawTurnInterface(int Miniturn){
    int Yloc = YWINSIZE - TURNBOXCOEF * 3;
    const float MaskOpacity = 0.5;
    ShadeWithColor(Blue,0,Yloc,XINTERFACE,Yloc + TURNBOXCOEF);
    pRT->DrawTextW(L"Build",5,pBuildForm,D2D1::RectF(0.2f * TURNBOXCOEF,Yloc + 0.15f * TURNBOXCOEF, 4.0f * TURNBOXCOEF, Yloc + 1.2f * TURNBOXCOEF),pBlackBrush);
    if(Miniturn != 0)
        ShadeWithColor(White,0,Yloc,XINTERFACE,Yloc + TURNBOXCOEF,MaskOpacity);

    Yloc += TURNBOXCOEF;
    ShadeWithColor(Green,0,Yloc,XINTERFACE,Yloc + TURNBOXCOEF);
    pRT->DrawTextW(L"Move",4,pMoveForm,D2D1::RectF(0.2f * TURNBOXCOEF, Yloc + 0.05f * TURNBOXCOEF, 5.0f * TURNBOXCOEF,  Yloc + 1.2f * TURNBOXCOEF),pBlackBrush);
    if(Miniturn != 1)
        ShadeWithColor(White,0,Yloc,XINTERFACE,Yloc + TURNBOXCOEF,MaskOpacity);

    Yloc += TURNBOXCOEF;
    ShadeWithColor(Red,0,Yloc,XINTERFACE,Yloc + TURNBOXCOEF);
    pRT->DrawTextW(L"Attack",6,pAttackForm,D2D1::RectF(0,Yloc + 0.25f * TURNBOXCOEF, 5.0f * TURNBOXCOEF,Yloc + 1.2f * TURNBOXCOEF),pBlackBrush);
    if(Miniturn != 2)
        ShadeWithColor(White,0,Yloc,XINTERFACE,Yloc + TURNBOXCOEF,MaskOpacity);
}
void ShadeSelectedBuilding(SelectTypes Build){
    int YSpot = static_cast<int>(Build) - BuildInterfaceHight;
    int ypix = BUILDINTERFACECOEF * YSpot;
    ShadeWithColor(Blue,0,ypix,XINTERFACE,ypix + BUILDINTERFACECOEF,0.5);
}
Color GetPlayerColor(int PlayerNum){
    switch(PlayerNum){
    case 0:return Red;
    case 1:return Blue;
    case 2:return Yellow;
    case 3:return Green;
    default: return Black;
    };
}
