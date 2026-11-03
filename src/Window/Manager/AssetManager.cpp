#include "AssetManager.h"

void AssetManager::Begin()
{
	Gdiplus::GdiplusStartupInput StartupInput;

	Gdiplus::Status Status = Gdiplus::GdiplusStartup(
		&GdiPlusToken,
		&StartupInput,
		nullptr
	);

	if (Status != Gdiplus::Ok)
	{
		// Error
	}

	LoadAsset();
}

void AssetManager::LoadAsset()
{

}