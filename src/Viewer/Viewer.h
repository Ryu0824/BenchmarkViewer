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
	private:
		void Init(Viewer* Instance)
		{
			if (!Instance) return;
			SelectTextBoxObject = Instance->FindObject(L"ResourceCurrentSelectBox")->GetComponent<TextBox>();
			MeasureMentListObject = Instance->FindObject(L"MeasureMentListObject")->GetComponent<ListBox>();
			EnviromentObject = Instance->FindObject(L"EnviromentListView")->GetComponent<ListView>();
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
		}

		void Reset()
		{
			SelectTextBoxObject->UpdateData(L"");
			MeasureMentListObject->Reset();
			EnviromentObject->Reset();
			MeasureMentListObject->Reset();
		}

	private:
		TextBox* SelectTextBoxObject = nullptr;
		ListView* EnviromentObject = nullptr;
		ListBox* MeasureMentListObject = nullptr;
		//Object* ChartListObject = nullptr;

		bool IsSelected = false;
		BenchMarkData Data;

		friend class Viewer;
	};

private:
	std::vector<Object*> Objects;
	std::vector<BenchMarkViewerSetting> Datas;

	Event<void(std::wstring)> ResourceAddEvent;
	Event<void()> ResourceRemoveEvent;

	LRESULT CurrentListIndex = 0;

	friend class BenchMarkViewerSetting;
};

