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

	int Left = Position.x - Scale.x / 2.f;
	int Top = Position.y - Scale.y / 2.f;
	int Right = Position.x + Scale.x / 2.f;
	int Bottom = Position.y + Scale.y / 2.f;

	Rectangle(dc, Left, Top, Right, Bottom);
}

void ChartGroup::End()
{

}