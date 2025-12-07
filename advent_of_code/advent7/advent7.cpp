#include "advent7.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY7DBG
#define DAY7DBG 1
#else
#define ENABLE_DAY7DBG 1
#ifdef NDEBUG
#define DAY7DBG 0
#else
#define DAY7DBG ENABLE_DAY7DBG
#endif
#endif

#if DAY7DBG
	#include <iostream>
#endif

namespace
{
#if DAY7DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "utils/dynamic_bits.h"
#include "utils/istream_line_iterator.h"

namespace
{
	constexpr char STARTING_MANIFOLD = 'S';
	constexpr char SPLITTER = '^';

	using LineData = utils::dynamic_bits<172>;

	LineData read_line(std::string_view line, char interesting_char)
	{
		return LineData{ line | std::views::transform([interesting_char](char c) {return c == interesting_char; }) };
	}

	LineData get_starting_lasers_p1(std::istream& input)
	{
		std::string line;
		std::getline(input, line);
		const LineData result = read_line(line, STARTING_MANIFOLD);
		AdventCheck(result.popcount() == 1u);
		return result;
	}

	std::string get_state_as_string(const LineData& splitters, const LineData& lasers)
	{
		const std::size_t width = std::max(splitters.size(), lasers.size());
		std::string result;
		result.reserve(width);
		for (std::size_t i = 0u; i < width; ++i)
		{
			if (i < splitters.size() && splitters.get_bit(i))
			{
				result.push_back('^');
			}
			else if (i < lasers.size() && lasers.get_bit(i))
			{
				result.push_back('|');
			}
			else
			{
				result.push_back(' ');
			}
		}
		return result;
	}

	std::size_t solve_p1(std::istream& input)
	{
		LineData lasers = get_starting_lasers_p1(input);
		std::size_t split_count = 0;
		log << '\n' << get_state_as_string({}, lasers);
		for (std::string_view line : utils::istream_line_range{ input })
		{
			const LineData splitters = read_line(line, SPLITTER);
			if (splitters.popcount() == 0u) continue;

			AdventCheck(lasers.size() == splitters.size());
			const auto split_lasers_mask = lasers & splitters;
			const auto num_splits = split_lasers_mask.popcount();
			if (num_splits == 0u) continue;

			split_count += num_splits;

			lasers = (lasers & ~split_lasers_mask) | (split_lasers_mask << 1) | (split_lasers_mask >> 1);
			log << '\n' << get_state_as_string(splitters, lasers);
		}

		log << '\n';

		return split_count;
	}
}

#include "utils/int_range.h"

namespace
{
	using TimelineCounter = utils::small_vector<uint64_t, 1>;
	TimelineCounter get_starting_lasers_p2(std::istream& input)
	{
		std::string line;
		std::getline(input, line);
		
		TimelineCounter result;
		result.reserve(line.size());

		stdr::transform(line, std::back_inserter(result), [](char c) -> uint64_t {return c == STARTING_MANIFOLD ? 1u : 0u; });
		return result;
	}

	void set_next_timeline(const TimelineCounter& current, TimelineCounter& next, std::string_view line)
	{
		AdventCheck(current.size() == line.size());
		next.clear();
		next.resize(current.size(), 0u);

		for (auto idx : utils::int_range{ std::ssize(current) })
		{
			const auto has_splitter = line[idx] == SPLITTER;
			if (has_splitter)
			{
				const auto left_idx = idx - 1;
				const auto right_idx = idx + 1;
				AdventCheck(left_idx >= 0);
				AdventCheck(right_idx < std::ssize(next));
				next[left_idx] += current[idx];
				next[right_idx] += current[idx];
			}
			else
			{
				next[idx] += current[idx];
			}
		}
	}

	uint64_t solve_p2(std::istream& input)
	{
		TimelineCounter timelines = get_starting_lasers_p2(input);
		TimelineCounter next_timeline;
		next_timeline.reserve(timelines.size());

		for (std::string_view line : utils::istream_line_range{ input })
		{
			set_next_timeline(timelines, next_timeline, line);
			timelines.swap(next_timeline);
		}

		return stdr::fold_left(timelines, uint64_t{ 0u }, std::plus<uint64_t>{});
	}
}

namespace
{
	constexpr const char* TESTCASE =
R"(.......S.......
...............
.......^.......
...............
......^.^......
...............
.....^.^.^.....
...............
....^.^...^....
...............
...^.^...^.^...
...............
..^...^.....^..
...............
.^.^.^.^.^...^.
...............)";
}

ResultType testcase_seven_a()
{
	auto input = std::istringstream{ TESTCASE };
	return solve_p1(input);
}

ResultType testcase_seven_b()
{
	auto input = std::istringstream{ TESTCASE };
	return solve_p2(input);
}

ResultType advent_seven_p1()
{
	auto input = advent::open_puzzle_input(7);
	return solve_p1(input);
}

ResultType advent_seven_p2()
{
	auto input = advent::open_puzzle_input(7);
	return solve_p2(input);
}

#undef DAY7DBG
#undef ENABLE_DAY7DBG
