#include "advent3.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY3DBG
#define DAY3DBG 1
#else
#define ENABLE_DAY3DBG 1
#ifdef NDEBUG
#define DAY3DBG 0
#else
#define DAY3DBG ENABLE_DAY3DBG
#endif
#endif

#if DAY3DBG
	#include <iostream>
#endif

namespace
{
#if DAY3DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include <algorithm>
#include <ranges>
#include <numeric>

#include "utils/istream_line_iterator.h"
#include "utils/to_value.h"

namespace
{
	template <int NUM_BATTERIES>
	void get_maximum_joltage_impl(std::string_view::iterator first, std::string_view::iterator last, char* destination_it, char* destination_end);

	template <>
	void get_maximum_joltage_impl<0>(std::string_view::iterator first, std::string_view::iterator last, char* destination_it, char* destination_end)
	{
		AdventCheck(destination_it != destination_end);
		*destination_it = '\0';
	}

	template <int NUM_BATTERIES>
	void get_maximum_joltage_impl(std::string_view::iterator first, std::string_view::iterator last, char* destination_it, char* destination_end)
	{
		AdventCheck(std::distance(first, last) >= NUM_BATTERIES);
		AdventCheck(std::distance(destination_it, destination_end) == (NUM_BATTERIES + 1));

		auto allowed_end = last - (NUM_BATTERIES - 1);
		auto best_battery = std::max_element(first, allowed_end);
		*destination_it = *best_battery;
		get_maximum_joltage_impl<NUM_BATTERIES-1>(best_battery + 1, last, destination_it + 1, destination_end);
	}
	
	template <int NUM_BATTERIES>
	uint64_t get_maximum_joltage(std::string_view battery_bank)
	{
		std::array<char, NUM_BATTERIES + 1> result_data;
		get_maximum_joltage_impl<NUM_BATTERIES>(begin(battery_bank), end(battery_bank), result_data.data(), result_data.data() + result_data.size());
		return utils::to_value<uint64_t>(result_data.data());
	}

	template <int NUM_BATTERIES>
	uint64_t solve_generic(std::istream& input)
	{
		const auto result = std::ranges::fold_left_first(utils::istream_line_range{ input } | std::views::transform(get_maximum_joltage<NUM_BATTERIES>), std::plus<uint64_t>{});
		AdventCheck(result.has_value());
		return *result;
	}
}

namespace
{
	uint64_t solve_p1(std::istream& input)
	{
		return solve_generic<2>(input);
	}

	uint64_t solve_p2(std::istream& input)
	{
		return solve_generic<12>(input);
	}
}

ResultType testcase_three_a(std::istream& input)
{
	return solve_p1(input);
}

ResultType testcase_three_b(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_three_p1()
{
	auto input = advent::open_puzzle_input(3);
	return solve_p1(input);
}

ResultType advent_three_p2()
{
	auto input = advent::open_puzzle_input(3);
	return solve_p2(input);
}

#undef DAY3DBG
#undef ENABLE_DAY3DBG
