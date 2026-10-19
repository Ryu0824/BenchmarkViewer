#pragma once

#include <vector>
#include <string>

class BenchMarkData
{
public:
	bool Parsing(const std::wstring& FilePath);

private:
	std::vector<std::vector<std::wstring>> BenchData;
};