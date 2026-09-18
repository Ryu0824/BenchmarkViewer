#pragma once
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace bench
{
	using Fields = std::map<std::string, std::string>;
	struct Row
	{
		Fields fields;
		bool summary = false;
		const std::string& Get(const std::string& key) const;
		std::optional<double> Number(const std::string& key) const;
		std::string RunKey() const;
	};
	struct Run { std::string key, label, description; };
	struct Scope { std::string run, test, size; };
	struct Summary
	{
		std::string backend;
		double minimum = 0, median = 0, maximum = 0;
		std::size_t sampleCount = 0;
		bool fromSummary = false;
	};
	struct Point { double round = 0;std::optional<double>value; };
	struct Series { std::string backend; std::vector<Point> points; };

	std::vector<Row> ParseCsv(std::string_view text);
	std::vector<Row> ReadCsv(const std::filesystem::path& path);
	std::string Format(double value);
	std::string MetricUnit(const std::string& metric);

	class Dataset
	{
	public:
		void Add(const std::vector<Row>& _rows);
		void Clear() { rows.clear(); }
		std::size_t SummaryCount()const;
		std::size_t SampleCount() const;
		std::vector<Run> Runs() const;
		std::vector<std::string> Cases(const std::string& run) const;
		std::vector<std::string> Sizes(const Scope& scope) const;
		std::vector<std::string> Metrics(const Scope& scope) const;
		std::vector<Summary> Summaries(const Scope& scope) const;
		std::vector<Series> Samples(const Scope& scope, const std::string& metric) const;
		std::string Export(const Scope& scope) const;
 
	private:
		std::map<std::string, Row> rows;
		std::vector<const Row*> Select(const Scope& scope) const;
	};
}