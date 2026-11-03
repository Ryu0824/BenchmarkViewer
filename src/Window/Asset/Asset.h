#pragma once

class Asset
{
public:
	virtual bool Load(const std::wstring& Path) = 0;
	virtual ~Asset() = default;

	void SetFilePath(const std::wstring& InFilePath) { FilePath = InFilePath; }
	std::wstring& GetFilePath() { return FilePath; }

private:
	std::wstring FilePath;
};