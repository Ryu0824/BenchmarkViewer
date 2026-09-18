#include "BenchmarkData/BenchmarkData.h"
#include "ChartScale.h"
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <limits>
#include <string>

namespace
{
	int checks = 0;
	void Check(bool ok, const std::string& message)
	{
		++checks;
		if (!ok)
			throw std::runtime_error(message);
	}

	void Near(double a, double b, const std::string& message)
	{
		Check(std::abs(a - b) <= 1e-8 * std::max(1.0, std::abs(b)), message);
	}

	template<class F>
	void Throws(F fn, const std::string& message)
	{
		bool threw = false;
		try
		{
			fn();
		}
		catch (const std::exception&)
		{
			threw = true;
		}

		Check(threw, message);
	}
}

int main(int argc, char** argv)
{
	try
	{
		using namespace bench;
		const std::string header = "run_id,case,sizes,backend,round,ns_per_pair\n";
		auto odd = ParseCsv("\xEF\xBB\xBF" + header + "r,test,16,CRT,1,9\r\nr,test,16,CRT,2,1\nr,test,16,CRT,3,5");
		Dataset small;
		small.Add(odd);
		Scope scope{ odd[0].RunKey(),"test","16" };
		Near(small.Summaries(scope)[0].median, 5, "odd median");
		small.Add(ParseCsv(header + "r,test,16,CRT,4,7"));
		Near(small.Summaries(scope)[0].median, 6, "even median");
		small.Add(ParseCsv(header + "r,test,16,CRT,4,100"));
		Check(small.SampleCount() == 4, "duplicate round replacement");
		Near(small.Summaries(scope)[0].median, 7, "median after duplicate replacement");
		auto quoted = ParseCsv(header + "r,\"a,b\nline\",16\"a\"\"b\",1,1e2");
		Check(quoted[0].Get("case") == "a,b\nline" && quoted[0].Get("backend") == "a\"b", "quoted comma, escaped quote and newline");
		Throws([&] {ParseCsv(header + "r,test,16,CRT,1,nan");}, "NaN rejected");
		Throws([&] {ParseCsv(header + "r,test,16,CRT,1,3junk");}, "trailing numeric text rejected");
		Throws([&] {ParseCsv(header + "r,test,16,CRT,1,-1");}, "negative time rejected");
		Throws([&] {ParseCsv(header + "r,test,16,CRT,1.2,5");}, "fractinal rund rejected");
		Throws([&] {ParseCsv(header + "r,test,16,CRT,0,5");}, "round zero rejected");
		Throws([&] {ParseCsv(header + "r,test,16,CRT,1");}, "missing cell rejected");
		Throws([&] {ParseCsv(header + "r,test,16,\"CRT,1,5");}, "unclosed quote rejected");
		Throws([&] {ParseCsv(header + "r,test,16,\"CRT\"x,1,5");}, "extra text after quote rejected");
		Throws([&] {ParseCsv("run_id,run_id\nr,r");}, "duplicate header rejected");
		Throws([&] {ParseCsv("run_id,case,sizes,backend,min_ns_per_pair,median_ns_per_pair,max_ns_per_pair\nr,t,1,b,5,4,6");}, "bad min median max rejected");
		Throws([&] {ParseCsv(header);}, "header only rejected");
		small.Add(ParseCsv(header + "r2,test,16,CRT,1,2"));
		Check(small.Runs().size() == 2, "run isolation");
		Near(small.Summaries(scope)[0].median, 7, "other run does not affect median");
		auto memory = ParseCsv(header.substr(0, header.size() - 1) + ",stats_available,probe_held_requested\nr,test,16,CRT,1,5,0,0\nr,test,16,CRT,2,6,1,0");
		Dataset stats;
		stats.Add(memory);
		auto points = stats.Samples(scope, "probe_held_requested")[0].points;
		Check(!points[0].value && points[1].value && *points[1].value == 0, "unavailable != genuine zero");
		ChartScale linear({ 0,100 }, false);Near(*linear.Fraction(50), .5, "linear scale");
		ChartScale log({ 0,1,1000 }, true); Check(!log.Fraction(0), "log zero omitted");
		Near(*log.Fraction(10), 1.0 / 3.0, "log spacing");
		ChartScale zeros({ 0,0 }, false);Check(std::isfinite(*zeros.Fraction(0)), "all zero axis");
		ChartScale empty({}, true); Check(std::isfinite(empty.Tick(.5)), "empty log axis");
		ChartScale huge({ std::numeric_limits<double>::max() }, false); Near(*huge.Fraction(std::numeric_limits<double>::max()), 1, "large finite values");
		ChartScale hugeLog({ std::numeric_limits<double>::max() }, true);
		Check(std::isfinite(hugeLog.Tick(1)), "large log tick remains finite");
		if (argc != 2) throw std::runtime_error("Pass the examples directory as the first argument.");
		const std::filesystem::path path(argv[1]);
		auto summary = ReadCsv(path / "benchmark.summary.csv"), samples = ReadCsv(path / "benchmark.samples.csv");
		Check(summary.size() == 48 && samples.size() == 576, "fixture record count");
		Dataset combined, computed; combined.Add(summary); combined.Add(samples); computed.Add(samples);
		combined.Add(summary); combined.Add(samples);
		Check(combined.SummaryCount() == 48 && combined.SampleCount() == 576, "fixture duplicate import");
		std::size_t comparisons = 0;
		for (const auto& run : combined.Runs())
			for (const auto& test : combined.Cases(run.key))
			{
				Scope s{ run.key,test,"" };
				for (const auto& size : combined.Sizes(s))
				{
					s.size = size;
					auto a = combined.Summaries(s), b = computed.Summaries(s);
					Check(a.size() == b.size(), "fixture backend count");
					for (std::size_t i = 0;i < a.size();++i)
					{
						Check(a[i].backend == b[i].backend, "fixture backend match");
						Near(a[i].minimum, b[i].minimum, "fixture minimum");
						Near(a[i].median, b[i].median, "fixture median");
						Near(a[i].maximum, b[i].maximum, "fixture maximum");
						Check(a[i].sampleCount == 12 && a[i].fromSummary && !b[i].fromSummary, "fixture source/count");
						++comparisons;
					}
					Dataset roundTrip; roundTrip.Add(ParseCsv(combined.Export(s)));
					auto exported = roundTrip.Summaries(s);
					Check(exported.size() == a.size(), "export round-trip row count");
					for (std::size_t i = 0;i < a.size();++i)
						Near(exported[i].median, a[i].median, "export preserves precision");
				}
			}
		Check(comparisons == 48, "all 48 allocator summaries compared");
		auto alternate = samples.front();
		alternate.fields["seed"] = "99999";
		combined.Add({ alternate });
		Check(combined.Runs().size() == computed.Runs().size() + 1, "same run ID different conditions isolated");
		std::cout << "PASS: " << checks << "checks; 48 summaries verified against 576 samples.\n";
		std::cout << "CSV validation, deduplication, condition isolation, missing stats, chart scales, export round-trip passed.\n";
		return 0;
	}
	catch (const std::exception& e)
	{
		std::cerr << "FAIL: " << e.what() << '\n';
		return 1;
	}
}