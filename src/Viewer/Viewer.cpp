#include "Viewer.h"

void Viewer::Begin(HWND WindowHandle)
{
	// 4 Object
	// 1-1. BenchMark Resource Add Button
	Object* ResourceAddBtn = new Object;
	Transform* TransformComp = ResourceAddBtn->AddComponent<Transform>();
	Button* ButtonComp = ResourceAddBtn->AddComponent<Button>();

	ButtonComp->SetClickEvent([&]()
		{
			this->OpenFile();
		});

	TransformComp->SetPosition(UMath::Vector2(75.f, 335.f));
	TransformComp->SetScale(UMath::Vector2(110.f, 50.f));

	Objects.push_back(ResourceAddBtn);

	// 1-2. BenchMark Resource Remove Button
	Object* ResourecRemoveBtn = new Object;
	TransformComp = ResourecRemoveBtn->AddComponent<Transform>();
	ButtonComp = ResourecRemoveBtn->AddComponent<Button>();

	ButtonComp->SetClickEvent([&]()
		{
			this->RemoveResource();
		});

	TransformComp->SetPosition(UMath::Vector2(185.f, 335.f));
	TransformComp->SetScale(UMath::Vector2(110.f, 50.f));

	Objects.push_back(ResourecRemoveBtn);

	// 1-3 BenchMark Resource Selected Button
	Object* ResourceSelectBtn = new Object;
	TransformComp = ResourceSelectBtn->AddComponent<Transform>();
	ButtonComp = ResourceSelectBtn->AddComponent<Button>();

	ButtonComp->SetClickEvent([&]()
		{
			this->SelectResource();
		});

	TransformComp->SetPosition(UMath::Vector2(295.f, 335.f));
	TransformComp->SetScale(UMath::Vector2(110.f, 50.f));

	Objects.push_back(ResourceSelectBtn);

	// 1-4. BenchMark Resource 
	Object* ResourceListBox = new Object;
	ResourceListBox->SetName(L"ResourceListBox");
	TransformComp = ResourceListBox->AddComponent<Transform>();
	ListBox* ResourceListBoxComp = ResourceListBox->AddComponent<ListBox>();
	ResourceListBoxComp->Begin(WindowHandle);

	TransformComp->SetPosition(UMath::Vector2(20.f, 100.f));
	TransformComp->SetScale(UMath::Vector2(300.f, 200.f));

	Objects.push_back(ResourceListBox);

	// 1-5. BenchMark Current Select Resource
	Object* ResourceCurrentSelectBox = new Object;
	ResourceCurrentSelectBox->SetName(L"ResourceCurrentSelectBox");
	TransformComp = ResourceCurrentSelectBox->AddComponent<Transform>();
	TextBox* ResourceTextBoxComp = ResourceCurrentSelectBox->AddComponent<TextBox>();
	ResourceTextBoxComp->Begin(WindowHandle);

	TransformComp->SetPosition(UMath::Vector2(20.f, 60.f));
	TransformComp->SetScale(UMath::Vector2(300.f, 20.f));

	Objects.push_back(ResourceCurrentSelectBox);

	// 2. BenchMark Enviroment
	Object* EnviromentListView = new Object;
	EnviromentListView->SetName(L"EnviromentListView");
	TransformComp = EnviromentListView->AddComponent<Transform>();
	ListView* EnviromentListViewComp = EnviromentListView->AddComponent<ListView>();
	EnviromentListViewComp->Begin(WindowHandle);

	TransformComp->SetPosition(UMath::Vector2(350.f, 20.f));
	TransformComp->SetScale(UMath::Vector2(700.f, 100.f));

	Objects.push_back(EnviromentListView);

	// 3. BenchMark Data Measurement
	Object* MeasureMentListObject = new Object;
	MeasureMentListObject->SetName(L"MeasureMentListObject");
	TransformComp = MeasureMentListObject->AddComponent<Transform>();
	ListBox* MeasureMentListBox = MeasureMentListObject->AddComponent<ListBox>();
	MeasureMentListBox->Begin(WindowHandle);

	TransformComp->SetPosition(UMath::Vector2(20.f, 370.f));
	TransformComp->SetScale(UMath::Vector2(300.f, 300.f));

	Objects.push_back(MeasureMentListObject);

	// BenchMark Chart
	Object* ChartGroupObject = new Object;
	ChartGroupObject->SetName(L"ChartGroupObject");
	TransformComp = ChartGroupObject->AddComponent<Transform>();
	ChartGroup* CharGroupComp = ChartGroupObject->AddComponent<ChartGroup>();

	TransformComp->SetPosition(UMath::Vector2(800.f, 400.f));
	TransformComp->SetScale(UMath::Vector2(800, 450.f));

	Objects.push_back(ChartGroupObject);

	// MeasureMent Button
	Object* MeasureMentAddBtn = new Object;
	TransformComp = MeasureMentAddBtn->AddComponent<Transform>();
	ButtonComp = MeasureMentAddBtn->AddComponent<Button>();

	ButtonComp->SetClickEvent([&]()
		{
			this->SelectMeasureMentChart();
		});

	TransformComp->SetPosition(UMath::Vector2(300.f, 200.f));
	TransformComp->SetScale(UMath::Vector2(110.f, 50.f));

	Objects.push_back(MeasureMentAddBtn);

	// CreateEvent
	// 1. ResourceAddEvent
	Callback<void(std::wstring)> AddCallback;
	AddCallback.Bind([&](std::wstring FilePath)
		{
			this->AddResource(FilePath);
		});
	ResourceAddEvent.Subscribe(AddCallback);
}

void Viewer::Update()
{
	for (auto object : Objects)
	{
		if (object)
			object->Update();
	}
}

void Viewer::Render(HDC dc)
{
	for (auto object : Objects)
	{
		if (object)
			object->Render(dc);
	}
}

void Viewer::End()
{
	for (auto object : Objects)
	{
		if (object)
		{
			object->End();
		}
	}
}

bool Viewer::OpenFile()
{
	wchar_t FilePath[32768] = {};

	OPENFILENAMEW Dialog = {};
	Dialog.lStructSize = sizeof(Dialog);
	Dialog.hwndOwner = nullptr;
	Dialog.lpstrFile = FilePath;
	Dialog.nMaxFile = static_cast<DWORD>(sizeof(FilePath) / sizeof(FilePath[0]));

	Dialog.lpstrFilter = L"\0*.csv";
	Dialog.lpstrTitle = L"Bench CSV File Select";
	Dialog.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

	if (!GetOpenFileNameW(&Dialog))
	{
		const DWORD error = CommDlgExtendedError();

		if (error != 0)
		{
			printf("File Selected Error : " + error + '\n');
		}

		return false;
	}

#ifdef _DEBUG
	// 경로 출력 로그
	printf("%ls\n", FilePath);
#endif 

	ResourceAddEvent.BroadCast(FilePath);
	return true;
}

void Viewer::AddResource(std::wstring FilePath)
{
	// 1. Parsing
	BenchMarkData Data;
	bool Result = Data.Parsing(FilePath);

	// error 시 로그 추가
	if (!Result)
	{
		return;
	}
	BenchMarkViewerSetting ViewerSetting;
	ViewerSetting.Data = Data;

	ViewerSetting.Init(this);

	Datas.push_back(ViewerSetting);

	// 2. Parsing 성공 시 ListBox 추가
	Object* ListBoxObject = FindObject(L"ResourceListBox");

	if (ListBoxObject)
	{
		ListBox* ListBoxComp = ListBoxObject->GetComponent<ListBox>();
		if (ListBoxComp)
		{
			ListBoxComp->AddRowData(FilePath);
		}
	}
}

void Viewer::RemoveResource()
{
	Object* ListBoxObject = FindObject(L"ResourceListBox");

	if (ListBoxObject)
	{
		ListBox* ListBoxComp = ListBoxObject->GetComponent<ListBox>();
		LRESULT Index = ListBoxComp->GetSelectedIndex();

		if (Index >= Datas.size()) return;

		BenchMarkViewerSetting BenchMarkData = Datas[Index];
		if (BenchMarkData.IsSelected)
		{
			BenchMarkData.Reset();
		}

		Datas.erase(Datas.begin() + Index);
		ListBoxComp->RemoveRowData();
	}
}

void Viewer::SelectResource()
{
	Object* ListBoxObject = FindObject(L"ResourceListBox");
	ListBox* ListBoxComp = ListBoxObject->GetComponent<ListBox>();

	if (ListBoxComp)
	{
		LRESULT Index = ListBoxComp->GetSelectedIndex();
		if (Index >= Datas.size())return;

		Datas[CurrentListIndex].IsSelected = false;
		CurrentListIndex = Index;
		Datas[Index].Selected();
	}
}

void Viewer::SelectMeasureMentChart()
{
	if (Datas.size() == 3)return;
	Object* MeasureMentObject = FindObject(L"MeasureMentListObject");
	ListBox* ListBoxComp = MeasureMentObject->GetComponent<ListBox>();

	if (ListBoxComp)
	{
		LRESULT Index = ListBoxComp->GetSelectedIndex();
		Datas[CurrentListIndex].PushChart(Index);
	}
}