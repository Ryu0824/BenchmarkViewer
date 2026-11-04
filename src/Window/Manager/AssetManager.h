#pragma once

#include <Windows.h>
#include <gdiplus.h>
#include <unordered_map>
#include <string>
#include "../Asset/Asset.h"

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

	template<typename T>
	void AddAsset(const std::wstring& FilePath)
	{
		std::unordered_map<std::wstring, Asset*>::iterator iter = Assets.find(FilePath);

		if (iter != Assets.end())
		{
#ifdef _DEBUG
			printf("AssetManager AddAsset Func : %ls Already Exists Key this Asset\n",FilePath.c_str());
#endif
			return;
		}

		T* Asset = new T;
		if (Asset->Load(FilePath))
		{
			Assets.insert({ FilePath, Asset });

#ifdef _DEBUG
			printf("AssetManager AddAsset Func : %ls Asset Load Successed!!\n", FilePath.c_str());
#endif
		}
	}

	template<typename T>
	T* GetAsset(const std::wstring& FilePath)
	{
		std::unordered_map<std::wstring, Asset*>::iterator iter = Assets.find(FilePath);

		if (iter == Assets.end())
		{
#ifdef _DEBUG
			printf("AssetManager GetAsset Func : %ls not Exists Key this Asset\n", FilePath.c_str());
#endif
			return nullptr;
		}

		if (!iter->second)
		{
#ifdef _DEBUG
			printf("AssetManager GetAsset Func : %ls is nullptr Asset\n", FilePath.c_str());
#endif
			return nullptr;
		}

		return dynamic_cast<T*>(iter->second);
	}

private:
	AssetManager() = default;
	~AssetManager() = default;

	void LoadAsset();

private:
	ULONG_PTR GdiPlusToken = 0;

	std::unordered_map<std::wstring, Asset*> Assets;
};