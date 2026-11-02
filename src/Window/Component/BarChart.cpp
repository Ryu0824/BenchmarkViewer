
#include "BarChart.h"
#include "../Object/Object.h"
#include "../Component/Transform.h"

void BarChart::Begin(HWND WindowHandle)
{
}

void BarChart::Update()
{
}

void BarChart::Render(HDC dc)
{
	Transform* TransformComp = GetOwner()->GetComponent<Transform>();
	RECT Client = {};
	if (TransformComp)
	{
		UMath::Vector2 Position = TransformComp->GetPosition();
		UMath::Vector2 Scale = TransformComp->GetScale();

		int Left = (int)(Position.x - Scale.x / 2.f);
		int Top = (int)(Position.y - Scale.y / 2.f);
		int Right = (int)(Position.x + Scale.x / 2.f);
		int Bottom = (int)(Position.y + Scale.y / 2.f);

		HPEN Pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));

		HGDIOBJ OldPen = SelectObject(dc, Pen);
		HGDIOBJ OldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));

		// 큰 배경
		Rectangle(dc, Left, Top, Right, Bottom);

		int Interval = 5;
		int SlotWidth = int((Scale.x - (float)(3 * Interval)) / 2.f);
		int SlotHeight = int(Scale.y - (float)(2 * Interval));

		int SlotIndex = 2;
		int SlotGraphGroup = 3;
		int GraphInstance = 3;

		int GraphIntervalX = 10;
		int GraphIntervalY = 10;

		int GraphGroupIntervalX = 50;

		int GraphInstanceInterval = 3;

		int MaxValue = 200;

		// 작은 슬롯 2개
		for (int i = 0;i < SlotIndex;i++)
		{
			int Slotleft = Left + Interval * (i + 1) + i * SlotWidth;
			int Slottop = (int)(Position.y - (float)SlotHeight / 2.f);
			int Slotright = Left + Interval * (i + 1) + (i + 1) * (SlotWidth);
			int Slotbottom = (int)(Position.y + (float)SlotHeight / 2.f);

			Rectangle(dc, Slotleft, Slottop, Slotright, Slotbottom);

			int GraphLineLeft = Slotleft + GraphIntervalX;
			int GraphLineTop = Slottop + GraphIntervalY;
			int GraphLineBottom = Slotbottom - GraphIntervalY;
			int GraphLineRight = Slotright - GraphIntervalX;

			MoveToEx(dc, GraphLineLeft, GraphLineTop, nullptr);
			LineTo(dc, GraphLineLeft, GraphLineBottom);
			LineTo(dc, GraphLineRight, GraphLineBottom);

			int GraphWidth = GraphLineRight - GraphLineLeft;

			int GraphGroupWidth = (GraphWidth - 3 * GraphGroupIntervalX) / SlotGraphGroup;

			// 그래프 그룹 3개
			for (int j = 0;j < SlotGraphGroup;j++)
			{
				int GraphGroupleft = int((float)GraphLineLeft + ((float)GraphGroupIntervalX / 2.f) * (float)(2 * j + 1) + (float)(j * GraphGroupWidth));
				int GraphGroupbottom = GraphLineBottom;

				int GraphInstanceWidth = (GraphGroupWidth - 2 * GraphInstanceInterval) / GraphInstance;
				int GraphInstanceHeight = GraphLineBottom - GraphLineTop;

				// 그래프 그룹 내 실제 그래프 인스턴스 3개
				for (int z = 0;z < GraphInstance; z++)
				{
					HPEN GraphInstancePen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
					HBRUSH GraphInstanceBrush = CreateSolidBrush(RGB(255, 0, 0));

					HGDIOBJ PrevPen = SelectObject(dc, GraphInstancePen);
					HGDIOBJ PrevBrush = SelectObject(dc, GraphInstanceBrush);

					int GraphInstanceleft = GraphGroupleft + (GraphInstanceInterval) * z + GraphInstanceWidth * z;
					int GraphInstancetop = int(GraphGroupbottom - ((float)(GraphInstanceHeight)*ChartData[i][j][z] / (float)MaxValue));
					int GraphInstanceright = GraphGroupleft + (GraphInstanceInterval) * z + GraphInstanceWidth * (z + 1);
					int GraphInstancebottom = GraphGroupbottom;

					Rectangle(dc, GraphInstanceleft, GraphInstancetop, GraphInstanceright, GraphInstancebottom);

					SelectObject(dc, PrevPen);
					SelectObject(dc, PrevBrush);
					DeleteObject(GraphInstancePen);
					DeleteObject(GraphInstanceBrush);
				}
			}
		}

		SelectObject(dc, OldPen);
		SelectObject(dc, OldBrush);
		DeleteObject(Pen);
	}
}

void BarChart::End()
{

}