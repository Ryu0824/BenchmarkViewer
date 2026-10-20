#include "BarChart.h"
#include "../Object/Object.h"
#include "../Component/Transform.h"

void BarChart::Begin(HWND WindowHandle)
{
	BarChartHandle = CreateWindowExW(
		WS_EX_CLIENTEDGE,
		L"BarChartControl",
		L"",
		WS_CHILD | WS_VISIBLE,
		20, 20,
		500, 300,
		WindowHandle,
		reinterpret_cast<HMENU>(1004),
		GetModuleHandleW(nullptr),
		nullptr
	);
}

void BarChart::Update()
{
	Transform* TransformComp = GetOwner()->GetComponent<Transform>();
	if (TransformComp)
	{
		UMath::Vector2 Position = TransformComp->GetPosition();
		UMath::Vector2 Scale = TransformComp->GetScale();

		MoveWindow(BarChartHandle,
			(int)Position.x, (int)Position.y,
			(int)Scale.x, (int)Scale.y,
			TRUE);
	}
}

void BarChart::Render(HDC dc)
{

}

void BarChart::End()
{

}