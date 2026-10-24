#pragma once

#include "Component.h"
#include <Windows.h>
#include <string>

class BarChart : public Component
{
public:
	virtual void Begin(HWND WindowHandle) override;
	virtual void Update() override;
	virtual void Render(HDC dc) override;
	virtual void End() override;

	static constexpr ComponentType GetComponentType()
	{
		return ComponentType::BarChart;
	}

	void PushData(float Data[2][3][3])
	{
		for (int i = 0;i < 2;i++)
		{
			for (int j = 0;j < 3;j++)
			{
				for (int z = 0;z < 3;z++)
				{
					ChartData[i][j][z] = Data[i][j][z];
				}
			}
		}
	}

private:
	HWND BarChartHandle;
	Object* Group;
	int Index;

	void SetGroup(Object* InGroup, const int InIndex)
	{
		Group = InGroup;
		Index = InIndex;
	}

	float ChartData[2][3][3];
	friend class ChartGroup;
};