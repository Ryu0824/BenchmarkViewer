#include "Viewer.h"
#include "../Window/Component/Button.h"
#include "../Window/Component/Transform.h"
#include "../Window/Component/ListBox.h"
#include "../BenchmarkData/BenchmarkData.h"

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

	// 1-3. BenchMark Resource 
	Object* ResourceListBox = new Object;
	ResourceListBox->SetName(L"ResourceListBox");
	TransformComp = ResourceListBox->AddComponent<Transform>();
	ListBox* ResourceListBoxComp = ResourceListBox->AddComponent<ListBox>();
	ResourceListBoxComp->Begin(WindowHandle);

	Objects.push_back(ResourceListBox);

	//TransformComp->SetPosition(UMath::Vector2(640.f, 360.f));
	//TransformComp->SetScale(UMath::Vector2(200.f, 200.f));

	// 2. BenchMark Data Measurement

	// BenchMark Enviroment

	// BenchMark Chart

	// CreateEvent
	// 1. ResourceAddEvent
	Callback<void(std::wstring)> AddCallback;
	AddCallback.Bind([&](std::wstring FilePath)
		{
			this->AddResource(FilePath);
		});
	ResourceAddEvent.Subscribe(AddCallback);

	// 2. ResourceRemoveEvent

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
			object->End();
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
#ifdef _DEBUG
		printf("Parsing 실패!!");
#endif 
		return;
	}
	// 2. Parsing 성공 시 ListBox 추가
	std::vector<Object*>::iterator iter = Objects.begin();
	Object* ListBoxObject = nullptr;
	for (;iter != Objects.end();iter++)
	{
		Object* FindObject = *iter;
		if (FindObject)
		{
			if (FindObject->GetName() == L"ResourceListBox")
			{
				ListBoxObject = FindObject;
				break;
			}
		}
	}

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
	std::vector<Object*>::iterator iter = Objects.begin();
	Object* ListBoxObject = nullptr;
	for (;iter != Objects.end();iter++)
	{
		Object* FindObject = *iter;
		if (FindObject)
		{
			if (FindObject->GetName() == L"ResourceListBox")
			{
				ListBoxObject = FindObject;
				break;
			}
		}
	}

	if (ListBoxObject)
	{
		ListBox* ListBoxComp = ListBoxObject->GetComponent<ListBox>();
		if (ListBoxComp)
		{
			ListBoxComp->RemoveRowData();
		}
	}
}