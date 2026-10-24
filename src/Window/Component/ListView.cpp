#include "ListView.h"
#include <CommCtrl.h>
#include "../Object/Object.h"
#include "../Component/Transform.h"

void ListView::Begin(HWND WindowHandle)
{
	ListViewHandle = CreateWindowExW(
		WS_EX_CLIENTEDGE,
		WC_LISTVIEWW,
		L"",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP |
		LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
		350, 20,
		700, 100,
		WindowHandle,
		reinterpret_cast<HMENU>(1003),
		GetModuleHandleW(nullptr),
		nullptr
	);

	if (!ListViewHandle) return;

	ListView_SetExtendedListViewStyle(
		ListViewHandle,
		LVS_EX_GRIDLINES,
		LVS_EX_FULLROWSELECT |
		LVS_EX_DOUBLEBUFFER
	);

	AddColumn(0, L"Platform", 100);
	AddColumn(1, L"Compiler", 100);
	AddColumn(2, L"Machine", 100);
}

void ListView::Update()
{
	Transform* TransformComp = GetOwner()->GetComponent<Transform>();
	if (TransformComp)
	{
		UMath::Vector2 Position = TransformComp->GetPosition();
		UMath::Vector2 Scale = TransformComp->GetScale();

		MoveWindow(ListViewHandle,
			(int)Position.x, (int)Position.y,
			(int)Scale.x, (int)Scale.y,
			TRUE);
	}
}

void ListView::Render(HDC dc)
{
}

void ListView::End()
{
}

void ListView::AddTableRow(const std::wstring Platform, const std::wstring Compiler, const std::wstring Machine)
{
	LVITEMW Item = {};
	Item.mask = LVIF_TEXT;
	Item.iItem = ListView_GetItemCount(ListViewHandle);
	Item.iSubItem = 0;
	Item.pszText = const_cast<wchar_t*>(Platform.c_str());

	const int RowIndex = static_cast<int>(
		SendMessageW(
			ListViewHandle,
			LVM_INSERTITEMW,
			0,
			reinterpret_cast<LPARAM>(&Item)
		)
		);

	if (RowIndex == -1)
	{
		return;
	}

	auto SetCell = [&](int ColumnIndex, const std::wstring Text)
		{
			LVITEMW Cell = {};
			Cell.iSubItem = ColumnIndex;
			Cell.pszText = const_cast<wchar_t*>(Text.c_str());

			SendMessageW(
				ListViewHandle,
				LVM_SETITEMTEXTW,
				static_cast<WPARAM>(RowIndex),
				reinterpret_cast<LPARAM>(&Cell)
			);
		};

	SetCell(1, Compiler);
	SetCell(2, Machine);
}

void ListView::Reset()
{
	ListView_DeleteAllItems(ListViewHandle);
}

void ListView::AddColumn(int Index, const std::wstring ColumnContent, int Width)
{
	LVCOLUMN Column = {};
	Column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
	Column.pszText = const_cast<wchar_t*>(ColumnContent.c_str());
	Column.cx = Width;
	Column.fmt = LVCFMT_LEFT;

	SendMessageW(ListViewHandle,
		LVM_INSERTCOLUMN,
		static_cast<WPARAM>(Index),
		reinterpret_cast<LPARAM>(&Column));
}