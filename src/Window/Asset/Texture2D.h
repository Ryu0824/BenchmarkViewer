#pragma once

#include "Asset.h"

class Texture2D :public Asset
{
public:
	Texture2D() = default;
	virtual ~Texture2D();
	virtual bool Load(const std::wstring& Path);

private:
	Gdiplus::Image Image;
};