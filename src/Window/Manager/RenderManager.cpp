#include "RenderManager.h"

void RenderManager::Begin(HDC OriginalDC, int width, int height)
{
	Width = width;
	Height = height;

	BackDC = CreateCompatibleDC(OriginalDC);

	BackBitmap = CreateCompatibleBitmap(OriginalDC, Width, Height);

	OldBitmap = SelectObject(BackDC, BackBitmap);
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
	DeleteObject(BackBitmap);
	DeleteDC(BackDC);
}