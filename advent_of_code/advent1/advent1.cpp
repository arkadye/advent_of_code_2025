#include "advent1.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY1DBG
#define DAY1DBG 1
#else
#define ENABLE_DAY1DBG 1
#ifdef NDEBUG
#define DAY1DBG 0
#else
#define DAY1DBG ENABLE_DAY1DBG
#endif
#endif

#if DAY1DBG
	#include <iostream>
#endif

namespace
{
#if DAY1DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "utils/modular_int.h"
#include "utils/istream_line_iterator.h"
#include "utils/to_value.h"

#include <numeric>

namespace
{
	constexpr int LOCK_SIZE = 100;
	constexpr int LOCKS_START = 50;
	using Password = int;
	using Lock = utils::modular<int>;
	Lock get_initial_lock()
	{
		return Lock{ LOCKS_START, LOCK_SIZE };
	}

	int get_direction_multiplier(char direction)
	{
		switch (direction)
		{
		case 'L': return -1;
		case 'R': return +1;
		default: AdventUnreachable();
		}
		return 0;
	}

	int get_offset(std::string_view line)
	{
		AdventCheck(!line .empty());
		const char direction = line[0];
		line.remove_prefix(1);

		const int distance = utils::to_value<int>(line);
		const int offset = get_direction_multiplier(direction) * distance;
		return offset;
	}

	template <AdventDay day>
	std::pair<Lock, Password> process_line(std::pair<Lock, Password> state, std::string_view line)
	{
		log << "\nProcessing line '" << line << "' (Pos=" << state.first << " Pwd=" << state.second << ')';
		const auto offset = get_offset(line);
		AdventCheck(offset != 0);
		const auto newPosition = state.first + offset;
		if constexpr (day == AdventDay::one)
		{
			state.second += (state.first ? 0 : 1);
		}
		if constexpr (day == AdventDay::two)
		{
			const auto steps_from_zero = [offset, start = state.first.get_value()]()
				{
					if (offset > 0)
					{
						return offset + start;
					}
					else
					{
						return -offset + (LOCK_SIZE - start) % LOCK_SIZE;
					}
					AdventUnreachable();
					return 0;
				}();

			state.second += steps_from_zero / LOCK_SIZE;
		}
		state.first = newPosition;
		return state;
	}

	template <AdventDay day>
	int64_t solve_generic(std::istream& input)
	{
		using ILI = utils::istream_line_iterator;
		const auto result = std::accumulate(ILI{ input }, ILI{}, std::pair<Lock, Password>{get_initial_lock(), 0}, process_line<day>);
		log << "\nFinal state: Pos=" << result.first << " Pwd=" << result.second;
		return result.second;
	}

	int64_t solve_p1(std::istream& input)
	{
		return solve_generic<AdventDay::one>(input);
	}
	int64_t solve_p2(std::istream& input)
	{
		return solve_generic<AdventDay::two>(input);
	}
}

const char* TEST_ONE_A = R"(L68
L30
R48
L5
R60
L55
L1
L99
R14
L82)";

ResultType testcase_one_a()
{
	std::istringstream input{ TEST_ONE_A };
	return solve_p1(input);
}

ResultType testcase_one_b()
{
	std::istringstream input{ TEST_ONE_A };
	return solve_p2(input);
}

ResultType advent_one_p1()
{
	auto input = advent::open_puzzle_input(1);
	return solve_p1(input);
}

ResultType advent_one_p2()
{
	auto input = advent::open_puzzle_input(1);
	return solve_p2(input);
}

#undef DAY1DBG
#undef ENABLE_DAY1DBG
