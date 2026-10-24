#pragma once

#include <vector>
#include "../Component/Component.h"
#include "../Object/Object.h"
#include "../Component/Transform.h"
#include "../Component/BarChart.h"

class ChartGroup : public Component
{
public:
	virtual void Begin(HWND WindowHandle);
	virtual void Update();
	virtual void Render(HDC dc);
	virtual void End();

	void PushChart(Object* Chart)
	{
		if (BarCharts.size() == 3)
		{
			// 경고 알림
			return;
		}
		BarChart* ChartComp = Chart->GetComponent<BarChart>();
		if (ChartComp)
		{
			ChartComp->SetGroup(GetOwner(), BarCharts.size());
			BarCharts.push_back(Chart);
		}
	}

	void RemoveChart(int Index)
	{
		if (BarCharts.empty() || BarCharts.size() <= (size_t)Index)
		{
			// 경고 알림
			return;
		}

		BarCharts.erase(BarCharts.begin() + Index);
	}

	static constexpr ComponentType GetComponentType()
	{
		return ComponentType::ChartGroup;
	}

	void Reset()
	{
		BarCharts.clear();
	}

private:
	std::vector<Object*> BarCharts;
	UMath::Vector2 Interval;
};