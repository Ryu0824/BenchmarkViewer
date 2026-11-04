#pragma once

#include "Asset.h"
#include <Windows.h>
#include <objidl.h>
#include <gdiplus.h>

class Texture2D :public Asset
{
public:
	Texture2D() = default;
	virtual ~Texture2D();
	virtual bool Load(const std::wstring& Path);

	Gdiplus::Image* GetSource() { return Source; }

private:
	Gdiplus::Image* Source;
};