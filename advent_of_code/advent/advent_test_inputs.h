#pragma once

#include <sstream>
#include <string_view>
#include <array>
#include <ranges>

namespace advent
{
	template <std::ranges::input_range T>
	std::string combine_inputs(const T& inputs, std::string delimiter = "\n")
	{
		std::ostringstream oss;
		std::ranges::copy(inputs, std::ostream_iterator<typename T::value_type>{oss, delimiter.c_str()});
		std::string result = oss.str();
		for (auto c : delimiter)
		{
			result.pop_back();
		}
		return result;
	}
}

/*

Define inline definitions of test inputs here for use in the testcases.

For example:

constexpr const char* TEST_ONE_A = "test1_input";

Use can also combine strings together with combine_inputs, for example, which will also add newlines automatically using the template parameter to decide how many:

constexpr const char* TEST_ONE_B = "test2";
static const auto TEST_ONE = advent::combine_inputs<2>(TEST_ONE_A, TEST_ONE_B);
will set TEST_ONE to "test1_input\n\ntest2"

*/

constexpr std::array<const char*,11> DAY_TWO
{
	"11-22",
	"95-115",
	"998-1012",
	"1188511880-1188511890",
	"222220-222224",
	"1698522-1698528",
	"446443-446449",
	"38593856-38593862",
	"565653-565659",
	"824824821-824824827",
	"2121212118-2121212124"
};

auto DAY_TWO_COMBINED = advent::combine_inputs(DAY_TWO, ",");

constexpr std::array<const char*, 4> DAY_THREE
{
	"987654321111111",
	"811111111111119",
	"234234234234278",
	"818181911112111"
};

auto DAY_THREE_COMBINED = advent::combine_inputs(DAY_THREE);