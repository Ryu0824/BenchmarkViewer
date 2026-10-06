#include "ConsoleManager.h"
#include <Windows.h>
#include <filesystem>

void ConsoleManager::Begin()
{
	if (AllocConsole())
	{
		FILE* Stream = nullptr;

		freopen_s(&Stream, "CONOUT$", "w", stdout);
		freopen_s(&Stream, "CONOUT$", "w", stderr);
		freopen_s(&Stream, "CONIN$", "r", stdin);

		printf("ConsoleManager Initialized!\n");
	}
}