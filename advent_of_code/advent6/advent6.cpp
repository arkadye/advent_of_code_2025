#include "advent6.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY6DBG
#define DAY6DBG 1
#else
#define ENABLE_DAY6DBG 1
#ifdef NDEBUG
#define DAY6DBG 0
#else
#define DAY6DBG ENABLE_DAY6DBG
#endif
#endif

#if DAY6DBG
	#include <iostream>
#endif

namespace
{
#if DAY6DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "utils/istream_line_iterator.h"
#include "utils/int_range.h"
#include "utils/trim_string.h"
#include "utils/split_string.h"
#include "utils/to_value.h"

#include <array>
#include <vector>

namespace
{
	enum class Operation : std::size_t
	{
		multiply = 0,
		add,
		NUM
	};

	Operation parse_op(char c)
	{
		using enum Operation;
		switch (c)
		{
		case '*': return multiply;
		case '+': return add;
		default: break;
		}
		AdventUnreachable();
		return NUM;
	}

	Operation parse_op(std::string_view sv)
	{
		AdventCheck(sv.size() == 1u);
		const char c = sv.front();
		return parse_op(c);
	}

	uint64_t update_value(uint64_t arg1, uint64_t arg2, Operation op)
	{
		using enum Operation;
		switch (op)
		{
		case multiply: return arg1 * arg2;
		case add:return arg1 + arg2;
		default: break;
		}
		AdventUnreachable();
		return 0u;
	}

	constexpr auto NUM_OPERATIONS = std::to_underlying(Operation::NUM);

	namespace p1_internal
	{

		using ResultAlternative = std::array<uint64_t, NUM_OPERATIONS>;
		using AllResults = std::vector<ResultAlternative>;

		void update_alternatives_value(ResultAlternative& ra, uint64_t arg)
		{
			for (auto idx : utils::int_range{ NUM_OPERATIONS })
			{
				uint64_t& value = ra[idx];
				value = update_value(value, arg, static_cast<Operation>(idx));
			}
		}

		void select_alternatives_value(ResultAlternative& ra, Operation op)
		{
			const auto op_idx = std::to_underlying(op);
			for (auto idx : utils::int_range{ NUM_OPERATIONS })
			{
				if (op_idx != idx)
				{
					ra[idx] = 0u;
				}
			}
		}

		uint64_t get_total_from_alternatives(const ResultAlternative& ra)
		{
			const uint64_t result = stdr::fold_left(ra, uint64_t{ 0 }, std::plus<uint64_t>{});
			AdventCheck(result == stdr::max(ra));
			return result;
		}

		uint64_t sum_all_results(const AllResults& vra)
		{
			const uint64_t result = stdr::fold_left(vra | stdr::views::transform(get_total_from_alternatives), uint64_t{ 0 }, std::plus<uint64_t>{});
			return result;
		}

		AllResults process_line(AllResults partial_results, std::string_view line)
		{
			const bool first_line = partial_results.empty();
			std::string_view fragment = utils::trim_string(line); // Easier to debug
			std::size_t num_sections = 0u;
			while (!fragment.empty())
			{
				const auto [arg, rest] = utils::split_string_at_first(fragment, ' ');

				AdventCheck(!arg.empty());
				AdventCheck(!std::isspace(arg.front()));
				AdventCheck(!std::isspace(arg.back()));

				if (utils::is_value(arg))
				{
					const auto val = utils::to_value<uint64_t>(arg);
					if (first_line)
					{
						auto& new_entry = partial_results.emplace_back();
						stdr::fill(new_entry, val);
					}
					else
					{
						update_alternatives_value(partial_results[num_sections], val);
					}
				}
				else
				{
					const Operation op = parse_op(arg);
					select_alternatives_value(partial_results[num_sections], op);
				}

				fragment = utils::trim_left(rest);
				++num_sections;
			}

			AdventCheck(num_sections == partial_results.size());
			return partial_results;
		}
	}

	uint64_t solve_p1(std::istream& input)
	{
		using namespace p1_internal;
		const AllResults calculation_results = stdr::fold_left(utils::istream_line_range{ input }, AllResults{}, process_line);
		const uint64_t result = sum_all_results(calculation_results);
		return result;
	}
}

namespace
{
	namespace p2_internal
	{
		struct Column
		{
			uint64_t val = std::numeric_limits<uint64_t>::max();
			bool is_set() const { return val != std::numeric_limits<uint64_t>::max(); }
#ifdef NDEBUG
			void end_column() {}
			void check_can_extend_column() {}
#else
			void end_column() { has_ended = true; }
			void check_can_extend_column() { AdventCheck(!has_ended); }
			bool has_ended = false;
#endif
		};
		using NumberColumns = std::vector<Column>;

		Column update_column(Column col, char val)
		{
			if (std::isspace(val))
			{
				if (col.is_set())
				{
					col.end_column();
				}
			}
			else if(std::isdigit(val))
			{
				col.check_can_extend_column();
				AdventCheck(val >= '0');
				const uint64_t increment = val - '0';
				if (!col.is_set())
				{
					col.val = increment;
				}
				else
				{
					col.val = 10 * col.val + increment;
				}
			}
			else
			{
				AdventUnreachable();
			}
			return col;
		}

		NumberColumns process_line(NumberColumns in, std::string_view line)
		{
			if (in.empty()) // It's the first line
			{
				in.resize(line.size());
			}

			if (stdr::none_of(line, [](char c) {return std::isdigit(c); })) // It's the final line! Process all the operations.
			{
				const auto num_calculations = stdr::count_if(line, [](char c) {return !std::isspace(c); });
				NumberColumns out;
				out.reserve(num_calculations);

				Operation current_op = Operation::NUM;
				for (auto [column, op_char] : stdr::zip_view(in, line))
				{
					if (!std::isspace(op_char))
					{
						AdventCheck(column.is_set());
						AdventCheck(current_op == Operation::NUM);
						current_op = parse_op(op_char);
						Column& next_val = out.emplace_back();
						next_val.val = column.val;
					}
					else
					{
						AdventCheck(current_op != Operation::NUM);
						if (column.is_set())
						{
							uint64_t& val = out.back().val;
							val = update_value(val, column.val, current_op);
						}
						else
						{
							current_op = Operation::NUM;
						}
					}
				}
				return out;
			}
			else
			{
				AdventCheck(in.size() == line.size());
				auto transfrom_fn = [](std::tuple<Column, char> arg)
					{
						const auto [column, character] = arg;
						return update_column(column, character);
					};

				stdr::transform(stdr::views::zip(in, line), begin(in), transfrom_fn);
				return in;
			}
			AdventUnreachable();
			return {};
		}
	}

	uint64_t solve_p2(std::istream& input)
	{
		using namespace p2_internal;
		const NumberColumns calculations = stdr::fold_left(utils::istream_line_range{ input }, NumberColumns{}, process_line);
		AdventCheck(stdr::all_of(calculations, &Column::is_set));
		const uint64_t result = stdr::fold_left(calculations | stdr::views::transform([](Column c) {return c.val; }), uint64_t{ 0u }, std::plus<uint64_t>{});
		return result;;
	}
}

namespace
{
	constexpr const char* TESTCASE =
R"(123 328  51 64 
 45 64  387 23 
  6 98  215 314
*   +   *   +  )";
}

ResultType testcase_six_a()
{
	std::istringstream input{ TESTCASE };
	return solve_p1(input);
}

ResultType testcase_six_b()
{
	std::istringstream input{ TESTCASE };
	return solve_p2(input);
}

ResultType advent_six_p1()
{
	auto input = advent::open_puzzle_input(6);
	return solve_p1(input);
}

ResultType advent_six_p2()
{
	auto input = advent::open_puzzle_input(6);
	return solve_p2(input);
}

#undef DAY6DBG
#undef ENABLE_DAY6DBG
