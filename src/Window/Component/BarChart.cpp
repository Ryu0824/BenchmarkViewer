
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
				int GraphGroupleft = int((float)GraphLineLeft + ((float)GraphGroupIntervalX / 2.f) * (float)((2 * j + 1) + j * GraphGroupWidth));
				int GraphGroupbottom = GraphLineBottom;

				//Rectangle(dc, GraphGroupleft, GraphGrouptop, GraphGroupright, GraphGroupbottom);

				int GraphInstanceWidth = (GraphGroupWidth - 2 * GraphInstanceInterval) / GraphInstance;
				int GraphInstanceHeight = GraphLineBottom - GraphLineTop;

				// 그래프 그룹 내 실제 그래프 인스턴스 3개
				for (int z = 0;z < GraphInstance;z++)
				{
					int GraphInstanceleft = GraphGroupleft + (GraphInstanceInterval)*z + GraphInstanceWidth * z;
					int GraphInstancetop = int(GraphGroupbottom - ((float)(GraphInstanceHeight)*ChartData[i][j][z] / (float)MaxValue));
					int GraphInstanceright = GraphGroupleft + (GraphInstanceInterval)*z + GraphInstanceWidth * (z + 1);
					int GraphInstancebottom = GraphGroupbottom;

					Rectangle(dc, GraphInstanceleft, GraphInstancetop, GraphInstanceright, GraphInstancebottom);
				}
			}
		}

		SelectObject(dc, OldPen);
		SelectObject(dc, OldBrush);
		DeleteObject(Pen);
	}

	//const int Values[] = { 40,80,60 };
	//constexpr int Count = 3;
	//constexpr int MaxValue = 100;

	//const int Left = 40;
	//const int Top = 20;
	//const int Bottom = Client.bottom - 30;
	//const int PlotWidth = Client.right - Left - 20;
	//const int PlotHeight = Bottom - Top;

	//if (PlotWidth >= Count && PlotHeight > 0)
	//{
	//	HGDIOBJ OldPen = SelectObject(dc, GetStockObject(BLACK_PEN));

	//	MoveToEx(dc, Left, Top, nullptr);
	//	LineTo(dc, Left, Bottom);
	//	LineTo(dc, Left + PlotWidth, Bottom);

	//	SelectObject(dc, OldPen);

	//	//HBRUSH Brush = CreateSolidBrush(RGB(70, 130, 220));

	//	//if (Brush)
	//	//{
	//	//	const int SlotWidth = PlotWidth / Count;
	//	//	const int BarWidth =
	//	//		SlotWidth > 1 ? SlotWidth / 2 : 1;

	//	//	for (int Index = 0;Index < Count;++Index)
	//	//	{
	//	//		const int Height =
	//	//			PlotHeight * Values[Index] / MaxValue;

	//	//		const int X =
	//	//			Left + Index * SlotWidth
	//	//			+ (SlotWidth - BarWidth) / 2;

	//	//		RECT Bar = {
	//	//			X,
	//	//			Bottom - Height,
	//	//			X + BarWidth,
	//	//			Bottom
	//	//		};

	//	//		FillRect(dc, &Bar, Brush);
	//	//	}

	//	//	DeleteObject(Brush);
	//	//}
}

void BarChart::End()
{

}