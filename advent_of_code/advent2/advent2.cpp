#include "advent2.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY2DBG
#define DAY2DBG 1
#else
#define ENABLE_DAY2DBG 1
#ifdef NDEBUG
#define DAY2DBG 0
#else
#define DAY2DBG ENABLE_DAY2DBG
#endif
#endif

#if DAY2DBG
	#include <iostream>
#endif

namespace
{
#if DAY2DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "utils/istream_line_iterator.h"
#include "utils/split_string.h"
#include "utils/to_value.h"
#include "utils/small_vector.h"
#include "utils/int_range.h"

#include <algorithm>
#include <ranges>
#include <numeric>
#ifndef NDEBUG
#include <source_location>
#endif

namespace
{
	uint64_t sum_invalid_ids_in_range_identical_length_and_divisor(uint64_t low_id, uint64_t high_id, uint64_t multiply_factor)
	{
		AdventCheck(std::to_string(low_id).size() == std::to_string(high_id).size());
		AdventCheck(low_id <= high_id);

		const uint64_t n_min = low_id % multiply_factor ? (low_id / multiply_factor + 1) : (low_id / multiply_factor);
		const uint64_t n_max = high_id / multiply_factor;

		/* If n goes 3 - 6 it forms a shape like so :
		*
		*     #
		*    ##
		*   ###
		*  ####
		*  ####
		*  ####
		*
		* This can be treated as a rectancle (base 1+6-3=4; hight 3; area=12) + a square (for M=(6-3)=3, area is (M * M+1) / 2 = 6) for a total of 18.
		* We can count the values and verify this is correct.
		*
		*/

		const uint64_t rectangle_area = [n_min, n_max]()
			{
				const auto base = 1 + n_max - n_min;
				const auto height = n_min;
				return base * height;
			}();

		const uint64_t triangle_area = [n_min, n_max]()
			{
				const auto base = n_max - n_min;
				return (base * (base + 1)) / 2;
			}();

		return multiply_factor * (rectangle_area + triangle_area);
	}

	// Return 0 if invalid.
	uint64_t get_multiplier(uint64_t section_length, uint64_t num_sections)
	{
		AdventCheck(num_sections >= 2u);
		uint64_t result = 1;
		for (uint64_t section = 0u; section < num_sections - 1; ++section)
		{
			for (uint64_t i = 0u; i < section_length; ++i)
			{
				result *= 10;
			}
			result += 1;
		}
		return result;
	}

	template <AdventDay day>
	uint64_t sum_invalid_ids_in_range_identical_length(std::string_view low_id, std::string_view high_id)
	{
		const auto id_length = low_id.length();
		AdventCheck(id_length == high_id.length());

		const uint64_t id_min = utils::to_value<uint64_t>(low_id);
		const uint64_t id_max = utils::to_value<uint64_t>(high_id);

		// For even length = L, we want to know how many numbers sit between low_id and high_id such that satisfy
		// integer solutions to:
		// ID = F * N).
		// Where F = 10^(L/2) + 1. This is a multiplier of a half-length ID that gives two repeating sections.
		// This means we can get values of N, using N = ID / F.
		// So if we get a lower and upper bound to N:
		// N(low) = ceil(low_id / F)
		// N(high) = floor(high_id / F)
		// Then we can take the ceiling of N(low) and floor of N(high).
		//
		// We can then get the sum of all N using some mathematical methods, and multiply this by F to get
		// the sum of all the IDs.

		if constexpr (day == AdventDay::one)
		{
			// Get a single multiplier half the length.
			constexpr int NUM_SECTORS = 2;

			if (id_length % NUM_SECTORS != 0u) return 0;
			
			const uint64_t sector_length = id_length / NUM_SECTORS;
			const uint64_t multiply_factor = get_multiplier(sector_length , NUM_SECTORS);
			return multiply_factor ? sum_invalid_ids_in_range_identical_length_and_divisor(id_min, id_max, multiply_factor) : 0;
		}

		if constexpr (day == AdventDay::two)
		{
			// In this situation we can get multipliers to test for repeated 1 sections, repeated 2 sections, and so on, up to half the length of the inputs.
			// But we must be careful. If we have an 8 digit ID: 42424242. It gets matched by checking for repeating 2 sections AND repeating 4 sections.
			// This means we must keep track per section length, and remove the double counted ones.
			// To do this, we go from the smallest possible length (1) to the largest (len / 2), and keep track of results at each length. Then we account for
			// any double-counting by removing any factors of the length. So if we check for length4 repeating sections, we go in and subtract the total for length2
			// and length1 to avoid multi-counting.

			constexpr std::size_t VECTOR_STACK_SIZE = 16;
#ifndef NDEBUG
			{
				std::source_location loc = std::source_location::current();
				const auto half_id_len = id_length / 2 + 1;
				if (half_id_len > VECTOR_STACK_SIZE)
				{
					log << std::format("\nWARNING! Increase the stack size of VECTOR_STACK_SIZE at {}:{}. Currently {} but needs at least {}", loc.file_name(), loc.line() - 3, VECTOR_STACK_SIZE, half_id_len);
				}
			}
#endif
			const uint64_t longest_length = id_length / 2;
			utils::small_vector<uint64_t, VECTOR_STACK_SIZE> lengths_checked(longest_length + 1, int64_t{ 0 });
			
			for (auto sector_len : utils::int_range<uint64_t>{1u, longest_length+1})
			{
				if (id_length % sector_len != 0)
				{
					continue;
				}
				const uint64_t num_sectors = id_length / sector_len;

				const uint64_t multiplier = get_multiplier(sector_len, num_sectors);
				uint64_t partial_result = sum_invalid_ids_in_range_identical_length_and_divisor(id_min, id_max, multiplier);

				for (auto prev_len : utils::int_range{ sector_len })
				{
					if (prev_len >= sector_len) break;
					if (prev_len == 0u) continue;
					if (sector_len % prev_len == 0u)
					{
						// We've double counted.
						partial_result -= lengths_checked[prev_len];
					}
				}

				lengths_checked[sector_len] = partial_result;
			}

			auto result = std::ranges::fold_left_first(lengths_checked, std::plus<uint64_t>{});
			AdventCheck(result.has_value());
			return result.value();
		}
		AdventUnreachable();
		return 0;
	}

	template <AdventDay day>
	uint64_t sum_invalid_ids_in_range(std::string_view low_id, std::string_view high_id)
	{
		AdventCheck(!low_id.empty());
		AdventCheck(!high_id.empty());
		AdventCheck(low_id.length() <= high_id.length());
		if (low_id.length() == high_id.length())
		{
			return sum_invalid_ids_in_range_identical_length<day>(low_id, high_id);
		}

		std::string dummy_high;
		dummy_high.reserve(high_id.length() - 1);
		dummy_high.assign(low_id.length(), '9');
		uint64_t partial_result = sum_invalid_ids_in_range_identical_length<day>(low_id, dummy_high);

		std::string dummy_low;
		dummy_low.reserve(high_id.length());
		dummy_low.assign(low_id.length() + 1, '0');
		dummy_low[0] = '1';

		while (dummy_low.length() < high_id.length())
		{
			dummy_high.push_back('9');
			partial_result += sum_invalid_ids_in_range_identical_length<day>(dummy_low, dummy_high);
			dummy_low.push_back('0');
		}

		partial_result += sum_invalid_ids_in_range_identical_length<day>(dummy_low, high_id);
		return partial_result;
	}

	template <AdventDay day>
	uint64_t solve_fragment(std::string_view input)
	{
		auto [low, high] = utils::split_string_at_first(input, '-');
		return sum_invalid_ids_in_range<day>(low, high);
	}

	template <AdventDay day>
	uint64_t solve_generic(std::istream& input)
	{
		auto fragments = std::views::transform(utils::istream_line_range{ input , ','}, solve_fragment<day>);
		auto result = std::ranges::fold_left_first(std::move(fragments), std::plus<uint64_t>{});
		AdventCheck(result.has_value());
		return *result;
	}
}

namespace
{
	uint64_t solve_p1(std::istream& input)
	{
		return solve_generic<AdventDay::one>(input);
	}

	int64_t solve_p2(std::istream& input)
	{
		return solve_generic<AdventDay::two>(input);
	}
}

ResultType testcase_two_a(std::istream& arg)
{
	return solve_p1(arg);
}

ResultType testcase_two_b(std::istream& arg)
{
	return solve_p2(arg);
}

ResultType advent_two_p1()
{
	auto input = advent::open_puzzle_input(2);
	return solve_p1(input);
}

ResultType advent_two_p2()
{
	auto input = advent::open_puzzle_input(2);
	return solve_p2(input);
}

#undef DAY2DBG
#undef ENABLE_DAY2DBG
