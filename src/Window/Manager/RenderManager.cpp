#include "RenderManager.h"

void RenderManager::Init(HDC OriginalDC, int InWidth, int InHeight)
{
	Width = InWidth;
	Height = InHeight;

	BackDC = CreateCompatibleDC(OriginalDC);

	BackBitmap = CreateCompatibleBitmap(OriginalDC, Width, Height);

	OldBitmap = SelectObject(BackDC, BackBitmap);

	BackGroundBrush = CreateSolidBrush(RGB(30, 30, 30));
}

void RenderManager::Begin()
{
	RECT rect = { 0,0,Width,Height };

	FillRect(BackDC, &rect, BackGroundBrush);
}

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
	SelectObject(BackDC, OldBitmap);
	DeleteObject(BackGroundBrush);
	DeleteObject(BackBitmap);
	DeleteDC(BackDC);
}