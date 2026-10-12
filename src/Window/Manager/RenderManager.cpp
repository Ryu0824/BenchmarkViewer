#include "RenderManager.h"

void RenderManager::Init(HWND WindowHandle, HDC OriginalDC)
{
	RECT Rect = {};
	GetClientRect(WindowHandle, &Rect);

	Width = (int)(Rect.right-Rect.left);
	Height = (int)(Rect.bottom - Rect.top);

	BackDC = CreateCompatibleDC(OriginalDC);

	BackBitmap = CreateCompatibleBitmap(OriginalDC, Width, Height);

	OldBitmap = SelectObject(BackDC, BackBitmap);

	BackGroundBrush = SelectObject(BackDC, GetStockObject(DC_BRUSH));

	BackGroundColor = SetDCBrushColor(BackDC, RGB(255, 0, 0));
}

// ClearRenderTarget
void RenderManager::Begin(BYTE R, BYTE G, BYTE B)
{
	SetDCBrushColor(BackDC, RGB(R, G, B));

	RECT rect = { 0,0,Width,Height };

	FillRect(BackDC, &rect, static_cast<HBRUSH>(GetStockObject(DC_BRUSH)));
}

// Present
void RenderManager::Render(HDC dc)
{
	BitBlt(
		dc,
		0, 0, Width, Height,
		BackDC,
		0, 0,
		SRCCOPY);
}

void RenderManager::End()
{
	SetDCBrushColor(BackDC, BackGroundColor);
	SelectObject(BackDC, BackGroundBrush);
	SelectObject(BackDC, OldBitmap);

	DeleteObject(BackGroundBrush);
	DeleteObject(BackBitmap);
	DeleteDC(BackDC);
}

void RenderManager::Resize(HDC OriginalDC, int InWidth, int InHeight)
{
	SelectObject(BackDC, OldBitmap);
	DeleteObject(BackBitmap);

	Width = InWidth;
	Height = InHeight;

	BackBitmap = CreateCompatibleBitmap(OriginalDC, InWidth, InHeight);
	OldBitmap = SelectObject(BackDC, BackBitmap);
}