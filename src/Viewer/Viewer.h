#pragma once

#include <vector>
#include "../Window/Object/Object.h"
#include <Windows.h>
#include <string>
#include "../Window/Manager/EventManager.h"

#include "../Window/Component/Button.h"
#include "../Window/Component/Transform.h"
#include "../Window/Component/ListBox.h"
#include "../Window/Component/TextBox.h"
#include "../Window/Component/ListView.h"
#include "../Window/Component/BarChart.h"
#include "../Window/Component/ChartGroup.h"
#include "../BenchmarkData/BenchmarkData.h"

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
	void SelectResource();

	void SelectMeasureMentChart();

	Object* FindObject(const std::wstring& Name)
	{
		std::vector<Object*>::iterator iter = Objects.begin();
		Object* ReturnObject = nullptr;
		for (;iter != Objects.end();iter++)
		{
			Object* FindObject = *iter;
			if (FindObject)
			{
				if (FindObject->GetName() == Name)
				{
					ReturnObject = FindObject;
					break;
				}
			}
		}

		return ReturnObject;
	}

private:
	class BenchMarkViewerSetting
	{
	public:
		~BenchMarkViewerSetting()
		{
			if (BarCharts.empty())return;

			for (size_t i = 0;i < BarCharts.size();++i)
			{
				if (BarCharts[i])
					delete BarCharts[i];
			}
		}

	private:
		void Init(Viewer* Instance)
		{
			if (!Instance) return;
			SelectTextBoxObject = Instance->FindObject(L"ResourceCurrentSelectBox")->GetComponent<TextBox>();
			MeasureMentListObject = Instance->FindObject(L"MeasureMentListObject")->GetComponent<ListBox>();
			EnviromentObject = Instance->FindObject(L"EnviromentListView")->GetComponent<ListView>();
			ChartGroupObject = Instance->FindObject(L"ChartGroupObject")->GetComponent<ChartGroup>();
		}

		void Selected()
		{
			IsSelected = true;

			Reset();

			SelectTextBoxObject->UpdateData(Data.GetFilePath());
			EnviromentObject->AddTableRow(Data.GetEnviromentData()[0], Data.GetEnviromentData()[1], Data.GetEnviromentData()[2]);

			for (size_t i = 0;i < Data.GetMeasurementData().size();++i)
			{
				MeasureMentListObject->AddRowData(Data.GetMeasurementData()[i]);
			}

			for (size_t i = 0;i < BarCharts.size();++i)
			{
				ChartGroupObject->PushChart(BarCharts[i]);
			}
		}

		void PushChart(int Index)
		{
			if (BarCharts.size() == 3)return;

			bool Duplicated = false;
			for (size_t i = 0;i < BarCharts.size();++i)
			{
				if (ChartNumber[i] == Index)
				{
					Duplicated = true;
					break;
				}
			}

			if (Duplicated)return;
			// Parsing Data 중 해당 Index 데이터 가져옴

			// Object 생성
			Object* Chart = new Object;
			Transform* TransformComp = Chart->AddComponent<Transform>();
			BarChart* BarChartComp = Chart->AddComponent<BarChart>();

			// Scale 맞추기
			TransformComp->SetPosition(UMath::Vector2(800.f, 400.f));
			TransformComp->SetScale(UMath::Vector2(800, 150.f));

			ChartGroupObject->PushChart(Chart);
			ChartNumber.push_back(Index);
			BarCharts.push_back(Chart);
		}

		void Reset()
		{
			SelectTextBoxObject->UpdateData(L"");
			MeasureMentListObject->Reset();
			EnviromentObject->Reset();
			MeasureMentListObject->Reset();
			ChartGroupObject->Reset();
		}

	private:
		TextBox* SelectTextBoxObject = nullptr;
		ListView* EnviromentObject = nullptr;
		ListBox* MeasureMentListObject = nullptr;
		ChartGroup* ChartGroupObject = nullptr;

		std::vector<Object*> BarCharts;
		std::vector<int> ChartNumber;

		bool IsSelected = false;
		BenchMarkData Data;

		friend class Viewer;
	};

private:
	std::vector<Object*> Objects;
	std::vector<BenchMarkViewerSetting> Datas;

	Event<void(std::wstring)> ResourceAddEvent;

	LRESULT CurrentListIndex = 0;

	friend class BenchMarkViewerSetting;
};

