#pragma once

#include <Windows.h>
#include <gdiplus.h>

class AssetManager
{
public:
	void Begin();
	void End();

	static AssetManager& GetInstance()
	{
		static AssetManager Instance;
		return Instance;
	}

	AssetManager(const AssetManager&) = delete;
	AssetManager(AssetManager&&) = delete;

	AssetManager& operator=(const AssetManager&) = delete;
	AssetManager& operator=(AssetManager&&) = delete;

private:
	AssetManager() = default;
	~AssetManager() = default;

	void LoadAsset();

private:
	ULONG_PTR GdiPlusToken = 0;
};