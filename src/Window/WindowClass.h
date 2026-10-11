#pragma once

#include "../Viewer/Viewer.h"

#include <Windows.h>

class WindowClass
{
public:
	void Begin(HINSTANCE Instance);
	void Update();
	void End();

private:
	void Resize(int InWidth, int InHeight);

	HWND WindowHandle;
	HDC DeviceContext;
	Viewer ViewInstance;
};