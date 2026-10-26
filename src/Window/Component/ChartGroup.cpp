#include "ChartGroup.h"

void ChartGroup::Begin(HWND WindowHandle)
{

}

void ChartGroup::Update()
{
	Transform* TransformComp = GetOwner()->GetComponent<Transform>();

	UMath::Vector2 Position = TransformComp->GetPosition();
	UMath::Vector2 Scale = TransformComp->GetScale();

	for (size_t i = 0;i < BarCharts.size();++i)
	{
		Transform* BarChartComp = BarCharts[i]->GetComponent<Transform>();

		UMath::Vector2 BarChartScale = BarChartComp->GetScale();

		BarChartComp->SetPosition(UMath::Vector2(Position.x, (Position.y - Scale.y / 2.f) + Interval.y * (i + 1) + (2 * i + 1) * (BarChartScale.y / 2.f)));
	}
}

void ChartGroup::Render(HDC dc)
{
	Transform* TransformComp = GetOwner()->GetComponent<Transform>();

	UMath::Vector2 Position = TransformComp->GetPosition();
	UMath::Vector2 Scale = TransformComp->GetScale();

	int Left = int(Position.x - Scale.x / 2.f);
	int Top = int(Position.y - Scale.y / 2.f);
	int Right = int(Position.x + Scale.x / 2.f);
	int Bottom = int(Position.y + Scale.y / 2.f);

	Rectangle(dc, Left, Top, Right, Bottom);

	if (BarCharts.empty()) return;

	for (size_t i = 0;i < BarCharts.size();++i)
	{
		BarChart* ChartComp = BarCharts[i]->GetComponent<BarChart>();
		if (ChartComp)
		{
			ChartComp->Render(dc);
		}
	}
}

void ChartGroup::End()
{

}