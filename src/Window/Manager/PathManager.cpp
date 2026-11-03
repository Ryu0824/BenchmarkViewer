#include "PathManager.h"

#include <Windows.h>
#include <filesystem>

void PathManager::Begin()
{
	std::wstring Buffer(32768, L'\0');

	const DWORD Length = GetModuleFileNameW(
		nullptr,
		Buffer.data(),
		static_cast<DWORD>(Buffer.size())
	);

	Buffer.resize(Length);

	ExeDirectory = std::filesystem::path(Buffer).parent_path();

#ifdef _DEBUG
	printf("PathManager Begin Function : Path Extract Completed-> Result : %ls\n", ExeDirectory.c_str());
#endif
}