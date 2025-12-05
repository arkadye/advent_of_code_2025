#include "advent5.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY5DBG
#define DAY5DBG 1
#else
#define ENABLE_DAY5DBG 1
#ifdef NDEBUG
#define DAY5DBG 0
#else
#define DAY5DBG ENABLE_DAY5DBG
#endif
#endif

#if DAY5DBG
	#include <iostream>
#endif

namespace
{
#if DAY5DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include <utility>
#include <algorithm>
#include <vector>

#include "utils/to_value.h"
#include "utils/split_string.h"
#include "utils/istream_line_iterator.h"
#include "utils/int_range.h"
#include "swap_remove.h"

namespace
{
	using ID = uint64_t;
	using Range = std::pair<ID, ID>;
	using RangeSet = std::vector<Range>;

	ID parse_id(std::string_view input)
	{
		return utils::to_value<ID>(input);
	}

	Range parse_range(std::string_view input)
	{
		const auto [low_str, high_str] = utils::split_string_at_first(input, '-');
		const ID low = parse_id(low_str);
		const ID high = parse_id(high_str);
		AdventCheck(low <= high);
		return Range{ low,high+1 };
	}

	bool ranges_overlap(const Range& a, const Range& b)
	{
		if (a.first > b.second) return false;
		if (a.second < b.first) return false;
		return true;
	}

	void add_range(RangeSet& range_set, Range new_range)
	{
		// Iterate all current ranges and eat any ranges we overlap with.
		for (auto it=begin(range_set);it!=end(range_set);/*I control this*/)
		{
			if (ranges_overlap(*it, new_range))
			{
				const bool is_last = std::distance(it, end(range_set)) == 1;
				new_range.first = std::min(new_range.first, it->first);
				new_range.second = std::max(new_range.second, it->second);
				utils::swap_remove(range_set, it);
				if (is_last) break;
			}
			else
			{
				++it;
			}
		}

		range_set.push_back(new_range);
	}

	RangeSet parse_ranges(std::istream& input)
	{
		std::string line;
		RangeSet result;
		while (true)
		{
			std::getline(input, line);
			if (line.empty()) break;
			add_range(result, parse_range(line));
		}
		stdr::sort(result);
		return result;
	}

	bool is_fresh(const RangeSet& fresh_ranges, ID id)
	{
		AdventCheck(std::numeric_limits<ID>::max() != id);
		const auto range_max = stdr::lower_bound(fresh_ranges, id+1, {}, &Range::first);
		const auto result = stdr::find_if(stdr::subrange{ cbegin(fresh_ranges) , range_max }, [id](ID high) {return id < high; }, &Range::second);
		return result != range_max;
	}

	std::size_t count_fresh(const RangeSet& fresh_ranges, std::istream& input)
	{
		auto pred = [&fresh_ranges](ID id)
			{
				return is_fresh(fresh_ranges, id);
			};

			return stdr::count_if(utils::istream_line_range{ input }, pred, parse_id);
	}

	uint64_t solve_p1(std::istream& input)
	{
		const RangeSet fresh_ranges = parse_ranges(input);
		return count_fresh(fresh_ranges, input);
	}
}

namespace
{
	uint64_t solve_p2(std::istream& input)
	{
		const RangeSet fresh_ranges = parse_ranges(input);
		return stdr::fold_left(fresh_ranges | stdr::views::transform([](const Range& r) {return r.second - r.first; }), uint64_t{ 0 }, std::plus<uint64_t>{});
	}
}

namespace
{
	constexpr const char* TESTCASE_A =
R"(3-5
10 - 14
16 - 20
12 - 18

1
5
8
11
17
32)";
}

ResultType testcase_five_a()
{
	auto input = std::istringstream{ TESTCASE_A };
	return solve_p1(input);
}

ResultType testcase_five_b()
{
	auto input = std::istringstream{ TESTCASE_A };
	return solve_p2(input);
}

ResultType advent_five_p1()
{
	auto input = advent::open_puzzle_input(5);
	return solve_p1(input);
}

ResultType advent_five_p2()
{
	auto input = advent::open_puzzle_input(5);
	return solve_p2(input);
}

#undef DAY5DBG
#undef ENABLE_DAY5DBG
