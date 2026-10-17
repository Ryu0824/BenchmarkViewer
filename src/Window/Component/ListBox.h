#pragma once

#include "Component.h"
#include <Windows.h>
#include <string>

class ListBox : public Component
{
public:
	virtual void Begin(HWND WindowHandle) override;
	virtual void Update() override;
	virtual void Render(HDC dc) override;
	virtual void End() override;

	void Reset();
	void AddRowData(const std::wstring& RowData);
	void RemoveRowData();

	static constexpr ComponentType GetComponentType()
	{
		return ComponentType::ListBox;
	}

private:
	HWND ListBoxHandle;
	LRESULT SelectedIndex;
};