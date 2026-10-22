#pragma once

#include <vector>
#include "../Object/Object.h"

class ChartGroup
{
public:
	virtual void Begin(HWND WindowHandle);
	virtual void Update();
	virtual void Render();
	virtual void End();

	void PushChart();
	void RemoveChart();

private:
	std::vector<Object*> BarChart;
};