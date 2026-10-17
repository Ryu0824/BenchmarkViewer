#pragma once

#include <vector>
#include "../Window/Object/Object.h"
#include <Windows.h>
#include <string>
#include "../Window/Manager/EventManager.h"

class BenchMarkData;

class Viewer
{
public:
	void Begin(HWND WindowHandle);
	void Update();
	void Render(HDC dc);
	void End();

private:
	bool OpenFile();

	void AddResource(std::wstring FilePath);
	void RemoveResource();

private:
	std::vector<Object*> Objects;
	std::vector<BenchMarkData> Datas;

	Event<void(std::wstring)> ResourceAddEvent;
	Event<void()> ResourceRemoveEvent;
};