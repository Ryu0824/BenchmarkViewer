#pragma once

#include <vector>
#include <string>
#include <unordered_map>

class BenchMarkData
{
public:
	bool Parsing(const std::wstring& InFilePath);

	std::wstring& GetFilePath() { return FilePath; }

	std::vector<std::wstring>& GetMeasurementData() { return MeasureMentData; }
	std::vector<std::wstring>& GetEnviromentData() { return EnviromentData; }
	//std::vector<std::wstring>& GetChartContent();

private:
	void EnviromentParsing();
	void MeasureMentParsing();
	void ChartConentParsing();

private:
	std::vector<std::vector<std::wstring>> RawData;

	std::vector<std::wstring> EnviromentData;
	std::vector<std::wstring> MeasureMentData;
	std::wstring FilePath;
};