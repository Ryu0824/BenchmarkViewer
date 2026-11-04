#include "AssetManager.h"
#include "PathManager.h"
#include <filesystem>

#include "../Asset/Texture2D.h"

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
	std::wstring AssetPath = PathManager::GetInstance().GetPath();

	AssetPath += L"\\Asset";

#ifdef _DEBUG
	printf("AssetManager LoadAsset Debug Log : Path -> %ls\n", AssetPath.c_str());
#endif

	// 현재는 이미지만 로드
	for (const auto& Entry : std::filesystem::directory_iterator(AssetPath))
	{
		if (!Entry.is_regular_file())
			continue;

		const std::filesystem::path FilePath = Entry.path();

		if (FilePath.extension() != L".png")
			continue;

#ifdef _DEBUG
		printf("AssetManager LoadAsset Debug Log : Image Path -> %ls\n", FilePath.c_str());
#endif
		AddAsset<Texture2D>(FilePath);
	}
}

void AssetManager::End()
{
	std::unordered_map<std::wstring, Asset*>::iterator Iter = Assets.begin();

	for (;Iter != Assets.end();)
	{
		delete Iter->second;
		Iter = Assets.erase(Iter);
	}

	if (GdiPlusToken != 0)
	{
		Gdiplus::GdiplusShutdown(GdiPlusToken);
		GdiPlusToken = 0;
	}
}