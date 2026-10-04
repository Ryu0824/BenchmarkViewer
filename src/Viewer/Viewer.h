#pragma once

#include <vector>
#include "../Window/Object/Object.h"
#include <Windows.h>

class Viewer
{
public:
	void Begin();
	void Update();
	void Render(HDC dc);
	void End();

private:
	std::vector<Object*> Objects;
};