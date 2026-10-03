#include "WindowClass.h"
#include <Windows.h>

LRESULT CALLBACK WindowProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
	Viewer* app = reinterpret_cast<Viewer*>(GetWindowLongPtrW(h, GWLP_USERDATA));
	if (m == WM_NCCREATE)
	{
		app = static_cast<Viewer*>(reinterpret_cast<CREATESTRUCTW*>(l)->lpCreateParams); app->window = h;
		SetWindowLongPtrW(h, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
	}
	try
	{
		return app ? app->Message(m, w, l) : DefWindowProcW(h, m, w, l);
	}
	catch (const std::exception& e)
	{
		MessageBoxW(h, Wide(e.what()).c_str(), L"Benchmark Viewer error", MB_OK | MB_ICONERROR);
		if (m == WM_CREATE)
			return -1;
		return 0;
	}
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
		return 1;
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