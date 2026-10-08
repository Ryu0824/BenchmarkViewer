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

	void Begin(HDC OriginalDC,int Width,int Height);
	void Render(HDC dc);
	void End();

private:
	RenderManager() = default;
	~RenderManager() = default;

	HDC BackDC;
	HBITMAP BackBitmap;
	HGDIOBJ OldBitmap;

	int Width;
	int Height;
};