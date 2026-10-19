#include "TextBox.h"

void TextBox::Begin(HWND WindowHandle)
{
	TextBoxHandle = CreateWindowExW(
		WS_EX_CLIENTEDGE,
		L"EDIT",
		L"",
		WS_CHILD | WS_VISIBLE | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT,
		20, 60,
		300, 20,
		WindowHandle,
		reinterpret_cast<HMENU>(1002),
		GetModuleHandleW(nullptr),
		nullptr
	);

	SendMessageW(TextBoxHandle, EM_SETREADONLY, TRUE, 0);
}

void TextBox::Update()
{
}

void TextBox::Render(HDC dc)
{

}

void TextBox::End()
{

}

void TextBox::UpdateData(const std::wstring SelectedData)
{
	SetWindowTextW(TextBoxHandle, SelectedData.c_str());
}