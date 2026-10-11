#pragma once

#include<Windows.h>

class RenderManager
{
public:
	static RenderManager& GetInstance()
	{
		static RenderManager Instance;
		return Instance;
	}

	RenderManager(const RenderManager&) = delete;
	RenderManager(RenderManager&&) = delete;

	RenderManager& operator=(const RenderManager&) = delete;
	RenderManager& operator=(RenderManager&&) = delete;

	HDC GetBackDC() { return BackDC; }

	void Init(HDC OriginalDC, int InWdith, int InHeight);

	void Begin(BYTE R, BYTE G, BYTE B);
	void Render(HDC dc);
	void End();

	void Resize(HDC OriginalDC, int InWidth, int InHeight);

private:
	RenderManager() = default;
	~RenderManager() = default;

	HGDIOBJ BackGroundBrush;
	COLORREF BackGroundColor;

	HDC BackDC;
	HBITMAP BackBitmap;
	HGDIOBJ OldBitmap;

	int Width;
	int Height;
};