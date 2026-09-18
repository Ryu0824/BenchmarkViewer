#pragma once
#include <algorithm>
#include <cmath>
#include <optional>
#include <limits>
#include <vector>
namespace bench
{
	struct ChartScale
	{
		bool logarithmic = false;
		double low = 0, high = 1;
		explicit ChartScale(const std::vector<double>& values, bool log) : logarithmic(log)
		{
			bool found = false;
			for (double v : values)
			{
				if (!std::isfinite(v) || (log && v <= 0))
					continue;
				double t = log ? std::log10(v) : v;
				if (!found)
				{
					low = log ? t : 0; high = t; found = true;
				}
				else
				{
					if (log) low = std::min(low, t); high = std::max(high, t);
				}
			}
			if (!found)
			{
				low = 0;
				high = 1;
			}
			if (log)
			{
				low = std::max(std::floor(low), std::log10(std::numeric_limits<double>::denorm_min()));
				high = std::min(std::ceil(high), std::log10(std::numeric_limits<double>::max()));
			}
			if (high <= low)
				high = low + 1;
		}

		std::optional<double> Fraction(double v) const
		{
			if (!std::isfinite(v) || (logarithmic && v <= 0))
				return{};
			const double t = logarithmic ? std::log10(v) : v;
			return std::clamp((t - low) / (high - low), 0.0, 1.0);
		}

		double Tick(double fraction) const
		{
			const double t = low + (high - low) * fraction;
			if (!logarithmic)
				return t;
			if (t >= std::log10(std::numeric_limits<double>::max()))
				return std::numeric_limits<double>::max();
			return std::pow(10.0, t);
		}
	};
}