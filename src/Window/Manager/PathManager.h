#pragma once

#include <string>

class PathManager
{
public:
	void Begin();

	static PathManager& GetInstance()
	{
		static PathManager Instance;
		return Instance;
	}

	PathManager(const PathManager&) = delete;
	PathManager(PathManager&&) = delete;

	PathManager& operator=(const PathManager&) = delete;
	PathManager& operator=(PathManager&&) = delete;

public:
	std::wstring& GetPath() { return ExeDirectory; }

private:
	PathManager() = default;
	~PathManager() = default;

	std::wstring ExeDirectory;
};