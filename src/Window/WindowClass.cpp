#include "WindowClass.h"
#include "Manager/InputManager.h"
#include "Manager/ConsoleManager.h"
#include "Manager/TimeManager.h"
#include "Manager/RenderManager.h"

LRESULT CALLBACK WindowProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
	switch (m)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProcW(h, m, w, l);
}

void WindowClass::Begin(HINSTANCE Instance)
{
#ifdef _DEBUG
	ConsoleManager::GetInstance().Begin();
#endif

	TimeManager::GetInstance().Begin();

	SetProcessDPIAware();

	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(wc);
	wc.hInstance = Instance;
	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = L"BenchMarkWindow";
	wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);

	if (!RegisterClassExW(&wc))
	{
		return;
	}

	WindowHandle = CreateWindowExW(0, L"BenchMarkWindow", L"BenchMarkViewer", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, nullptr, nullptr, Instance, nullptr);

	ShowWindow(WindowHandle, SW_SHOW);

	DeviceContext = GetDC(WindowHandle);

	ViewInstance.Begin();

	RenderManager::GetInstance().Init(DeviceContext, 1280, 720);
}

void WindowClass::Update()
{
	TimeManager::GetInstance().Update();

	InputManager::GetInstance().Update(WindowHandle);

	ViewInstance.Update();

	RenderManager::GetInstance().Begin();

	ViewInstance.Render(RenderManager::GetInstance().GetBackDC());

	RenderManager::GetInstance().Render(DeviceContext);
}

void WindowClass::End()
{
	RenderManager::GetInstance().End();

	ReleaseDC(WindowHandle, DeviceContext);

	ViewInstance.End();
}