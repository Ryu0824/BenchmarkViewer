#include "BenchmarkData.h"
#include <fstream>
#include <iostream>
#include <sstream>

bool BenchMarkData::Parsing(const std::wstring& FilePath)
{
	std::wifstream File(FilePath.c_str());

	if (!File.is_open())
	{
#ifdef _DEBUG
		printf("BenchMarkData : File Parsing is Failed!");
#endif
		return false;
	}

	std::wstring Line;

	while (std::getline(File, Line))
	{
		if (!Line.empty() && Line.back() == L'\r')
		{
			Line.pop_back();
		}

		std::wstringstream Stream(Line);
		std::vector<std::wstring> Row;
		std::wstring Cell;

		while (std::getline(Stream, Cell, L','))
		{
			Row.push_back(Cell);
		}

		BenchData.push_back(Row);
	}

	int a = 0;

	return true;
}