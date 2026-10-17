#include "ListBox.h"

void ListBox::Begin(HWND WindowHandle)
{
	ListBoxHandle = CreateWindowExW(
		WS_EX_CLIENTEDGE,
		L"LISTBOX",
		L"",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT,
		20, 100,
		300, 200,
		WindowHandle,
		reinterpret_cast<HMENU>(1001),
		GetModuleHandleW(nullptr),
		nullptr
	);
}

void ListBox::Update()
{
}

void ListBox::Render(HDC dc)
{

}

void ListBox::End()
{

}

void ListBox::Reset()
{
	SendMessageW(ListBoxHandle, LB_RESETCONTENT, 0, 0);
}

void ListBox::AddRowData(const std::wstring& RowData)
{
	SendMessageW(ListBoxHandle, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(RowData.c_str()));
}

void ListBox::RemoveRowData()
{
	SelectedIndex = SendMessageW(ListBoxHandle, LB_GETCURSEL, 0, 0);

	if (SelectedIndex != LB_ERR)
	{
		SendMessageW(ListBoxHandle, LB_DELETESTRING, static_cast<WPARAM>(SelectedIndex), 0);
	}
}
