#pragma once

#include "Component.h"
#include <Windows.h>
#include <string>

class ListView : public Component
{
public:
	virtual void Begin(HWND WindowHandle) override;
	virtual void Update() override;
	virtual void Render(HDC dc) override;
	virtual void End() override;

	static constexpr ComponentType GetComponentType()
	{
		return ComponentType::ListView;
	}

	void AddTableRow(const std::wstring Platform, const std::wstring Compiler, const std::wstring Machine);
	void Reset();

private:
	void AddColumn(int Index, const std::wstring ColumnContent, int Width);

private:
	HWND ListViewHandle;
};