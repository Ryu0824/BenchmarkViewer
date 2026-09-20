#include "BenchmarkData.h"
#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <set>
#include <sstream>
#include <stdexcept>

namespace bench
{
	namespace
	{
		constexpr std::array<const char*, 13> RunFiels =
		{
			"run_id","platform","compiler","machine","logical_threads","seed",
			"slots","cycles","repeats","warmups","alignment","touch_policy","allocation_pairs"
		};

		std::string Pack(std::string_view s) { return std::to_string(s.size()) + ':' + std::string(s); }

		// Remove side margins
		std::string_view Trim(std::string_view s)
		{
			while (!s.empty() && (s.front() == ' ' || s.front() == '\t'))s.remove_prefix(1);
			while (!s.empty() && (s.back() == ' ' || s.back() == '\t'))s.remove_suffix(1);
			return s;
		}	

		// Extract Number
		std::optional<double> Numeric(std::string_view s)
		{
			s = Trim(s);
			if (s.empty())return{};
			double v = 0;
			auto [end, ec] = std::from_chars(s.data(), s.data() + s.size(), v);
			if (ec != std::errc{} || end != s.data() + s.size() || !std::isfinite(v))return{};
			return v;
		}

		// Status is check
		bool IsStat(const std::string& key)
		{
			return key.starts_with("probe_") || key.starts_with("after_") || key.starts_with("timed_page_");
		}

		// Available Stat check
		std::optional<double> Metric(const Row& row, const std::string& key)
		{
			if (IsStat(key) && row.Get("stats_available") != "1")return{};
			return row.Number(key);
		}


		std::string Quote(std::string s)
		{
			if (!s.empty() && (s[0] == '=' || s[0] == '+' || s[0] == '-' || s[0] == '@' || s[0] == '\t' || s[0] == '\r')) s.insert(s.begin(), '\'');
			std::string out = "\"";
			for (char c : s) { out += c; if (c == '"')out += '"'; }
			return out + '"';
		}

		std::string Exact(double v)
		{
			std::ostringstream out; out.imbue(std::locale::classic());
			out << std::setprecision(17) << v; return out.str();
		}
	}

	const std::string& Row::Get(const std::string& key) const
	{
		static const std::string empty;
		auto iter = fields.find(key); 
		return iter == fields.end() ? empty : iter->second;
	}

	std::optional<double>Row::Number(const std::string& key) const { return Numeric(Get(key)); }

	std::string Row::RunKey() const
	{
		std::string out;
		for (auto key : RunFiels) out += Pack(Get(key));
		return out;
	}

	std::vector<Row> ParseCsv(std::string_view text)
	{
		if (text.starts_with("\xEF\xBB\xBF")) 
			text.remove_prefix(3);

		if (text.find('\0') != std::string_view::npos) 
			throw std::runtime_error("NULL byte detected. SAVE CSV as UTF-8, not UTF-16.");
		 
		enum class State { Start, Plain, Quoted, AfterQuote } 
		state = State::Start;

		std::vector<std::vector<std::string>> table;
		std::vector<std::string> row;
		std::string cell;
		auto finishRow = [&]{
				row.push_back(std::move(cell)); cell.clear();
				if (std::any_of(row.begin(), row.end(), [](const auto& v) {return !Trim(v).empty();})) table.push_back(std::move(row));
				row.clear();
			};

		for (std::size_t i = 0;i < text.size();++i)
		{
			char c = text[i];
			if (state == State::Quoted)
			{
				if (c == '"')
				{
					if (i + 1 < text.size() && text[i + 1] == '"') { cell += '"';++i; }
					else state = State::AfterQuote;
				}
				else
					cell += c;
				continue;
			}

			if (c == ',' || c == '\r' || c == '\n')
			{
				if (c == ',') { row.push_back(std::move(cell));cell.clear(); }
				else { finishRow(); if (c == '\r' && i + 1 < text.size() && text[i + 1] == '\n')++i; }
				state = State::Start;
				continue;
			}

			if (state == State::AfterQuote) 
				throw std::runtime_error("Unexpected character after closing CSV quote");

			if (c == '"')
			{
				if (state != State::Start)
					throw std::runtime_error("Unexpected qutoe in unquoted CSV field");
				state = State::Quoted;
			}
			else
			{
				cell += c;
				state = State::Plain;
			}
		}

		if (state == State::Quoted) throw std::runtime_error("Unclosed CSV quote");

		finishRow();

		if (table.size() < 2)
			throw std::runtime_error("CSV needs a header and at least one data record.");

		auto header = table.front();

		std::set<std::string> names;
		for (auto& h : header)
		{
			h = std::string(Trim(h));
			if (h.empty() || !names.insert(h).second) 
				throw std::runtime_error("Empty or duplicate CSV header");
		}

		bool summary = names.contains("median_ns_per_pair");

		for (auto k : { "run_id","case","sizes","backend" })
		{
			if (!names.contains(k)) 
				throw std::runtime_error(std::string("Missing column: ") + k);
		}

		std::vector<std::string> numeric = summary
			? std::vector<std::string>{"min_ns_per_pair", "median_ns_per_pair", "max_ns_per_pair"}
			: std::vector<std::string>{"round", "ns_per_pair" };

		for (const auto& k : numeric) 
			if (!names.contains(k)) 
				throw std::runtime_error("Missing column" + k);

		std::vector<Row> out;
		for (std::size_t i = 1;i < table.size();++i)
		{
			const auto prefix = "Record" + std::to_string(i + 1) + ": ";

			if (table[i].size() != header.size()) 
				throw std::runtime_error(prefix + "column count mismatch");

			Row r; r.summary = summary;
			for (std::size_t j = 0;j < header.size();++j) r.fields.emplace(header[j], table[i][j]);
			for (auto k : { "run_id","case","sizes","backend" })
				if (Trim(r.Get(k)).empty()) throw std::runtime_error(prefix + "empty key: " + k);
			for (const auto& k : numeric)
			{
				auto n = r.Number(k);
				if (!n || *n < 0)throw std::runtime_error(prefix + "invalid nonnegative number: " + k);
			}
			if (!summary)
			{
				const double round = *r.Number("round");
				if (round < 1 || round>9007199254740991.0 || std::floor(round) != round)
					throw std::runtime_error(prefix + "round mush be a positive integer");
			}
			else if (*r.Number("min_ns_per_pair") > *r.Number("median_ns_per_pair") ||
				*r.Number("median_ns_per_pair") > *r.Number("max_ns_per_pair"))
				throw std::runtime_error(prefix + "expectd min <= median <= max.");
			out.push_back(std::move(r));
		}
		return out;
	}

	// File Context Copy
	std::vector<Row> ReadCsv(const std::filesystem::path& path)
	{
		if (std::filesystem::file_size(path) > 64ull * 1024 * 1024) 
			throw std::runtime_error("CSV exceeds the 64 MiB file limit.");

		std::ifstream in(path, std::ios::binary);
		if (!in)
			throw std::runtime_error("Cannot open file.");
		
		std::string text{ std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() };
		
		if (in.bad()) 
			throw std::runtime_error("Failed to read file");

		return ParseCsv(text);
	}

	std::string Format(double value)
	{
		std::ostringstream out; out.imbue(std::locale::classic());
		if ((value != 0 && std::abs(value) < .001) || std::abs(value) >= 1e9) out << std::scientific;
		else out << std::fixed;
		out << std::setprecision(3) << value;
		return out.str();
	}

	std::string MetricUnit(const std::string& m)
	{
		if (m == "ns_per_pair") return "ns/pair";
		if (m == "elapsed_ns")return "ns";
		if (m.find("requested") != std::string::npos || m.find("capacity") != std::string::npos ||
			m.find("data") != std::string::npos || m.find("metadata") != std::string::npos)return "bytes";
		return "count";
	}

	void Dataset::Add(const std::vector<Row>& _rows)
	{
		for (const auto& r : _rows)
		{
			auto key = Pack(r.RunKey()) + Pack(r.Get("case")) + Pack(r.Get("sizes")) + Pack(r.Get("backend"));
			key += r.summary ? "summary" : "samples" + Exact(*r.Number("round"));
			this->rows.insert_or_assign(key, r);
		}
	}

	std::size_t Dataset::SummaryCount() const
	{
		return static_cast<std::size_t>(std::count_if(rows.begin(), rows.end(), [](const auto& p) {return p.second.summary;}));
	}

	std::size_t Dataset::SampleCount() const { return rows.size() - SummaryCount(); }

	std::vector<Run> Dataset::Runs() const
	{
		std::map<std::string, Run> result;
		for (const auto& [key, r] : rows)
		{
			(void)key;
			auto run = r.RunKey();
			if (result.contains(run)) continue;
			std::string desc;
			for (auto field : RunFiels) desc += std::string(field) + "=" + r.Get(field) + " ";
			result.emplace(run, Run{ run, r.Get("run_id") + " | " + r.Get("platform") + " | " + r.Get("compiler") + " | slots=" + r.Get("slots") + " | pairs=" + r.Get("allocation_pairs"), desc });
		}
		std::vector<Run> out;
		for (const auto& [key, r] : result) { (void)key;out.push_back(r); }
		for (std::size_t i = 0;i < out.size();++i) out[i].label = '[' + std::to_string(i + 1) + "] " + out[i].label;
		return out;
	}

	std::vector<const Row*> Dataset::Select(const Scope& s)const
	{
		std::vector<const Row*> out;
		for (const auto& [key, r] : rows)
		{
			(void)key;
			if (r.RunKey() == s.run && r.Get("case") == s.test && r.Get("sizes") == s.size)out.push_back(&r);
		}
		return out;
	}

	std::vector<std::string> Dataset::Cases(const std::string& run) const
	{
		std::set<std::string> result;
		for (const auto& [key, r] : rows) { (void)key;if (r.RunKey() == run) result.insert(r.Get("case")); }
		return { result.begin(), result.end() };
	}

	std::vector<std::string> Dataset::Sizes(const Scope& s) const
	{
		std::set<std::string> result;
		for (const auto& [key, r] : rows) { (void)key; if (r.RunKey() == s.run && r.Get("case") == s.test) result.insert(r.Get("sizes")); }
		std::vector<std::string> out{ result.begin(),result.end() };
		std::sort(out.begin(), out.end(), [](const auto& a, const auto& b)
			{
				auto na = Numeric(a), nb = Numeric(b);
				if (na && nb && *na != *nb)return *na < *nb;
				if (bool(na) != bool(nb))return bool(na);
				return a < b;
			});
		return out;
	}

	std::vector<std::string> Dataset::Metrics(const Scope& s) const
	{
		std::set<std::string> result;
		for (const auto* r : Select(s)) if (!r->summary)
			for (const auto& [k, v] : r->fields)
			{
				(void)v;
				if ((k == "elapsed_ns" || k.starts_with("held_") || IsStat(k)) && Metric(*r, k)) result.insert(k);
			}
		std::vector<std::string> out{ "ns_per_pair" }; out.insert(out.end(), result.begin(), result.end());return out;
	}

	std::vector<Summary> Dataset::Summaries(const Scope& s) const
	{
		std::map<std::string, const Row*> summary;
		std::map<std::string, std::vector<double>> samples;
		std::set<std::string> backends;
		for (const auto* r : Select(s))
		{
			const auto& b = r->Get("backend"); backends.insert(b);
			if (r->summary) summary[b] = r;
			else samples[b].push_back(*r->Number("ns_per_pair"));
		}
		std::vector<Summary> out;
		for (const auto& b : backends)
		{
			auto& v = samples[b];
			Summary a; a.backend = b; a.sampleCount = v.size();
			if (summary.contains(b))
			{
				const Row& r = *summary[b]; a.fromSummary = true;
				a.minimum = *r.Number("min_ns_per_pair"); a.median = *r.Number("median_ns_per_pair");
				a.maximum = *r.Number("max_ns_per_pair");
			}
			else
			{
				std::sort(v.begin(), v.end());
				const auto n = v.size(); a.minimum = v.front(); a.maximum = v.back();
				a.median = v[(n - 1) / 2] / 2 + v[n / 2] / 2;
			}
			out.push_back(a);
		}
		return out;
	}

	std::vector<Series> Dataset::Samples(const Scope& s, const std::string& metric) const
	{
		std::map<std::string, std::vector<Point>> groups;
		for (const auto* r : Select(s)) if (!r->summary)
			groups[r->Get("backend")].push_back({ *r->Number("round"),Metric(*r,metric) });
		std::vector<Series> out;
		for (auto& [b, p] : groups)
		{
			std::sort(p.begin(), p.end(), [](auto a, auto c) {return a.round < c.round;});
			out.push_back({ b,std::move(p) });
		}
		return out;
	}

	std::string Dataset::Export(const Scope& s) const
	{
		const auto _rows = Select(s);
		if (_rows.empty()) throw std::runtime_error("No selected data to export.");
		std::string out = "\xEF\xBB\xBF";
		for (auto key : RunFiels) out += std::string(key) + ',';
		out += "case,sizes,backend,min_ns_per_pair,median_ns_per_pai,max_ns_per_pair,sample_count,source\r\n";
		for (const auto& a : Summaries(s))
		{
			for (auto key : RunFiels)
				out += Quote(_rows.front()->Get(key)) + ',';
			out += Quote(s.test) + ',' + Quote(s.size) + ',' + Quote(a.backend) + ',' + Exact(a.minimum) + ',' + Exact(a.median) + ',' + Exact(a.maximum) + ',' + std::to_string(a.sampleCount) + ',' + (a.fromSummary ? "summary" : "samples") + "\r\n";
		}
		return out;
	}
}