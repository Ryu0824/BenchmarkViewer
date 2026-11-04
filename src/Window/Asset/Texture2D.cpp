#include "Texture2D.h"

Texture2D::~Texture2D()
{
	if (Source)
	{
		delete Source;
		Source = nullptr;
	}
}

bool Texture2D::Load(const std::wstring& Path)
{
	Source = new Gdiplus::Image(Path.c_str());

	if (Source->GetLastStatus() != Gdiplus::Ok)
	{
		if (Source)
		{
#ifdef _DEBUG
			printf("Texture2D Load Func : Path %ls Image Load Failed!!\n", Path.c_str());
#endif
			delete Source;
			Source = nullptr;
		}
		return false;
	}

	return true;
}