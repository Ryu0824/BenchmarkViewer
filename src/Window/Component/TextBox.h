#pragma once

#include "Component.h"
#include <Windows.h>
#include <string>

class TextBox : public Component
{
public:
	virtual void Begin(HWND WindowHandle) override;
	virtual void Update() override;
	virtual void Render(HDC dc) override;
	virtual void End() override;

	static constexpr ComponentType GetComponentType()
	{
		return ComponentType::TextBox;
	}

	void UpdateData(const std::wstring SelectedData);

private:
	HWND TextBoxHandle;
};