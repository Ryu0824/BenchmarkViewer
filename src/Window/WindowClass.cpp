#include "WindowClass.h"

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
}

void WindowClass::Update()
{

}

void WindowClass::End()
{

}