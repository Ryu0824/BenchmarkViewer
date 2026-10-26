#ifdef _DEBUG
#include <cstdlib>
#include <crtdbg.h>
#endif // _DEBUG

#include "Window/WindowClass.h"

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prevInstance, PWSTR, int show)
{
#ifdef _DEBUG

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);

	const int Flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

	_CrtSetDbgFlag(
		Flags |
		_CRTDBG_ALLOC_MEM_DF |
		_CRTDBG_LEAK_CHECK_DF
	);

	//_CrtSetBreakAlloc(93);
#endif
	WindowClass Class;

	Class.Begin(instance);

	MSG msg{};

	bool Running = true;

	while (Running)
	{
		Class.BeginFrame();

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				Running = false;
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (!Running)
			break;

		Class.Update();
	}

	Class.End();


	return 0;
}