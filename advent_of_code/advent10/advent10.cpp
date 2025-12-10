#include "advent10.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY10DBG
#define DAY10DBG 1
#else
#define ENABLE_DAY10DBG 1
#ifdef NDEBUG
#define DAY10DBG 0
#else
#define DAY10DBG ENABLE_DAY10DBG
#endif
#endif

#if DAY10DBG
	#include <iostream>
#endif

namespace
{
#if DAY10DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "utils/dynamic_bits.h"
#include "utils/small_vector.h"
#include "utils/string_line_iterator.h"
#include "utils/istream_line_iterator.h"
#include "utils/parse_utils.h"
#include "utils/to_value.h"
#include "utils/int_range.h"
#include "utils/hash.h"

#include <vector>
#include <unordered_set>

namespace
{
	using Lights = utils::dynamic_bits<64>;
	using Button = utils::dynamic_bits<64>;
	using Buttons = utils::small_vector<Button, 16>;
	using JoltCounters = utils::small_vector<int16_t, 16>;
}
	
namespace std
{
	template <>
	struct hash<JoltCounters>
	{
		std::size_t operator()(const JoltCounters& jc) const noexcept
		{
			std::size_t result = 0u;
			
			auto it = begin(jc);
			while (it != end(jc))
			{
				std::size_t intermediate = 0u;
				for (std::size_t i = 0u; (i < sizeof(std::size_t)) && it != end(jc); i += sizeof(JoltCounters::value_type), ++it)
				{
					intermediate = intermediate << ((CHAR_BIT) * sizeof(JoltCounters::value_type));
					intermediate += *it;
				}

				if (result == 0u)
				{
					result = intermediate;
				}
				else
				{
					result = utils::hash_combine(result, intermediate);
				}
			}
			return result;
		}
	};
}

namespace
{
	constexpr char LIGHT_ON = '#';

	struct Machine
	{
		Lights lights;
		Buttons buttons;
		JoltCounters joltCounters;
	};

	std::string_view remove_brackets(std::string_view in, char open, char close)
	{
		in = utils::trim_string(in);
		in = utils::remove_specific_prefix(in, open);
		in = utils::remove_specific_suffix(in, close);
		return in;
	}

	Lights parse_lights(std::string_view bit)
	{
		bit = remove_brackets(bit, '[', ']');

		const Lights result(bit | stdr::views::transform([](char c) {return c == LIGHT_ON; }));
		return result;
	}

	JoltCounters parse_jolt_counters(std::string_view bit)
	{
		bit = remove_brackets(bit, '{', '}');
		JoltCounters result;
		stdr::transform(utils::string_line_range{ bit,',' }, std::back_inserter(result), utils::to_value<int16_t>);
		return result;
	}

	Button parse_button(std::string_view bit, std::size_t lights_size)
	{
		bit = remove_brackets(bit, '(', ')');
		Button result;
		result.resize(lights_size);
		for (auto idx : utils::string_line_range(bit, ',') | stdr::views::transform(utils::to_value<std::size_t>))
		{
			result.set_bit(idx, true);
		}
		return result;
	}

	template <AdventDay day>
	Machine parse_machine(std::string_view line)
	{
		utils::small_vector<std::string_view, 16> fragments;
		stdr::copy(utils::string_line_range{ line, ' ' }, std::back_inserter(fragments));
		AdventCheck(fragments.size() >= 3u);

		Machine result;
		if constexpr (day == AdventDay::one)
		{
			result.lights = parse_lights(fragments.front());
		}

		result.joltCounters = parse_jolt_counters(fragments.back());
		fragments.pop_back();

		result.buttons.reserve(fragments.size() - 1);

		const auto width = [&result]()
			{
				switch (day)
				{
				case AdventDay::one:
					return result.lights.size();
				case AdventDay::two:
					return result.joltCounters.size();
				}
				return std::size_t{};
			}();

		auto trans = [width](std::string_view sv) {return parse_button(sv, width); };
		stdr::transform(fragments | std::views::drop(1), std::back_inserter(result.buttons), trans);
		return result;
	}

	template <AdventDay DAY>
	int64_t get_fewest_button_presses(const Machine& machine);

	template <>
	int64_t get_fewest_button_presses<AdventDay::one>(const Machine& machine)
	{
		using ButtonIt = decltype(begin(machine.buttons));
		using State = std::pair<Lights, ButtonIt>;

		std::vector<State> states_to_check{ std::pair{machine.lights,begin(machine.buttons)} };
		std::vector<State> next_states_to_check;

		int64_t depth = 0;
		while (true)
		{
			for (const State& state : states_to_check)
			{
				for (auto it = state.second; it != end(machine.buttons); ++it)
				{
					const Button& button = *it;
					AdventCheck(button.size() == state.first.size());
					const Lights next_lights = state.first ^ button;
					if (next_lights.popcount() == 0u)
					{
						return depth + 1;
					}
					next_states_to_check.emplace_back(next_lights, it + 1);
				}
			}

			states_to_check.swap(next_states_to_check);
			next_states_to_check.clear();
			++depth;
		}
		AdventUnreachable();
		return 0;
	}

	JoltCounters press_button(JoltCounters counters, Button button)
	{
		for (auto idx : utils::int_range{ button.size() })
		{
			AdventCheck(idx < counters.size());
			if (button.get_bit(idx))
			{
				--counters[idx];
			}
		}
		return counters;
	}

	template <>
	int64_t get_fewest_button_presses<AdventDay::two>(const Machine& machine)
	{
		// TODO
		return 0;
	}

	template <AdventDay day>
	int64_t solve_line(std::string_view line)
	{
		Machine machine = parse_machine<day>(line);
		return get_fewest_button_presses<day>(machine);
	}

	template <AdventDay day>
	int64_t solve(std::istream& input)
	{
		return stdr::fold_left(utils::istream_line_range{ input } | std::views::transform(solve_line<day>), int64_t{ 0 }, std::plus<int64_t>{});
	}
}

ResultType testcase_ten_a(std::istream& input)
{
	return solve<AdventDay::one>(input);
}

ResultType testcase_ten_b(std::istream& input)
{
	return solve<AdventDay::two>(input);
}

ResultType advent_ten_p1()
{
	auto input = advent::open_puzzle_input(10);
	return solve<AdventDay::one>(input);
}

ResultType advent_ten_p2()
{
	auto input = advent::open_puzzle_input(10);
	return solve<AdventDay::two>(input);
}

#undef DAY10DBG
#undef ENABLE_DAY10DBG
