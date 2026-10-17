#include "WindowClass.h"
#include "Manager/InputManager.h"
#include "Manager/ConsoleManager.h"
#include "Manager/TimeManager.h"
#include "Manager/RenderManager.h"
#include "Manager/EventManager.h"

LRESULT CALLBACK WindowProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
	switch (m)
	{
	case WM_SIZE:
	{
		if (w == SIZE_MINIMIZED)
			return 0;

		int Width = LOWORD(l);
		int Height = HIWORD(l);

		if (Width > 0 && Height > 0)
		{
			EventManager::GetInstance().Get<void()>(EventID::WindowResize).BroadCast();
			printf("WindowProc :: Window Reisze!\n");
		}

		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		InputManager::GetInstance().OnKeyDown(VK_LBUTTON);
		return 0;
	}
	case WM_LBUTTONUP:
	{
		InputManager::GetInstance().OnKeyUp(VK_LBUTTON);
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		InputManager::GetInstance().OnKeyDown(VK_RBUTTON);
		return 0;
	}
	case WM_RBUTTONUP:
	{
		InputManager::GetInstance().OnKeyUp(VK_RBUTTON);
		return 0;
	}
	case WM_KEYDOWN:
	{
		InputManager::GetInstance().OnKeyDown(static_cast<unsigned int>(w));
		return 0;
	}
	case WM_KEYUP:
	{
		InputManager::GetInstance().OnKeyUp(static_cast<unsigned int>(w));
		return 0;
	}
	case WM_SYSKEYDOWN:
	{
		InputManager::GetInstance().OnKeyDown(static_cast<unsigned int>(w));
		break;
	}
	case WM_SYSKEYUP:
	{
		InputManager::GetInstance().OnKeyUp(static_cast<unsigned int>(w));
		break;
	}
	case WM_KILLFOCUS:
	{
		InputManager::GetInstance().Reset();
		break;
	}
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

	Event<void()>& ResizeEvent = EventManager::GetInstance().Create<void()>(EventID::WindowResize);

	Callback<void()> ResizeCallback;

	ResizeCallback.Bind([this]()
		{
			this->Resize();
		}
	);

	ResizeEvent.Subscribe(ResizeCallback);

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

	WindowHandle = CreateWindowExW(0, L"BenchMarkWindow", L"BenchMarkViewer", WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, nullptr, nullptr, Instance, nullptr);

	DeviceContext = GetDC(WindowHandle);

	RenderManager::GetInstance().Init(WindowHandle, DeviceContext);

	ShowWindow(WindowHandle, SW_SHOW);

	ViewInstance.Begin(WindowHandle);
}

void WindowClass::BeginFrame()
{
	InputManager::GetInstance().Begin();
}

void WindowClass::Update()
{
	TimeManager::GetInstance().Update();

	InputManager::GetInstance().Update(WindowHandle);

	ViewInstance.Update();

	RenderManager::GetInstance().Begin(255, 255, 255);

	ViewInstance.Render(RenderManager::GetInstance().GetBackDC());

	RenderManager::GetInstance().Render(DeviceContext);
}

void WindowClass::End()
{
	RenderManager::GetInstance().End();

	ReleaseDC(WindowHandle, DeviceContext);

	ViewInstance.End();
}

void WindowClass::Resize()
{
	RECT Rect = {};

	if (!GetClientRect(WindowHandle, &Rect))
	{
		return;
	}

	RenderManager::GetInstance().Resize(DeviceContext, (int)(Rect.right - Rect.left), (int)(Rect.bottom - Rect.top));
}