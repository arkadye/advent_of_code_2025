#include "advent4.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY4DBG
#define DAY4DBG 1
#else
#define ENABLE_DAY4DBG 1
#ifdef NDEBUG
#define DAY4DBG 0
#else
#define DAY4DBG ENABLE_DAY4DBG
#endif
#endif

#if DAY4DBG
	#include <iostream>
#endif

namespace
{
#if DAY4DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include <algorithm>
#include "utils/grid.h"

namespace
{
	constexpr int64_t INACCESSIBLE_THRESHOLD = 4;
	constexpr char STACK = '@';
	constexpr char SPACE = '.';

	bool is_valid_map_node(char c)
	{
		switch (c)
		{
		case STACK:
		case SPACE:
			return true;
		default:
			break;
		}
		return false;
	}

	using Map = utils::grid<char>;
	auto get_map_coords_range(const Map& map)
	{
		return utils::coords_iterators::elem_range{ map.get_max_point() };
	}

	Map get_map(std::istream& input)
	{
		const Map result = utils::grid_helpers::build(input);
		AdventCheck(stdr::all_of(get_map_coords_range(result),
			is_valid_map_node,
			[&result](const auto& coords) {return result.at(coords); }));
		return result;
	}

	bool has_stack(const Map& map, const utils::coords& location)
	{
		return map.is_on_grid(location) && map[location] == STACK;
	}

	bool is_accessible_stack(const Map& map, const utils::coords& location)
	{
		if (!has_stack(map, location)) return false;
		const auto neighbours = location.neighbours_plus_diag();
		auto has_stack_pred = [&map](const utils::coords& nloc) {return has_stack(map, nloc); };
		const auto stacks = stdr::count_if(neighbours, has_stack_pred);
		return stacks < INACCESSIBLE_THRESHOLD;
	}

	int64_t get_accessible_coordinate_count(const Map& map)
	{
		auto is_accessible_pred = [&map](const utils::coords& loc)
			{
				return is_accessible_stack(map, loc);
			};

		return stdr::count_if(get_map_coords_range(map), is_accessible_pred);
	}

	int64_t solve_p1(std::istream& input)
	{
		return get_accessible_coordinate_count(get_map(input));
	}
}

namespace
{
	int64_t solve_p2(std::istream& input)
	{
		Map map = get_map(input);

		auto get_accessible_stacks_predicate = [&map](const utils::coords& loc)
			{
				return is_accessible_stack(map, loc);
			};

		auto coords_range = get_map_coords_range(map);
		int64_t num_removed = 0;

		while (true)
		{
			bool stack_removed = false;
			for (const utils::coords& loc : coords_range | std::views::filter(get_accessible_stacks_predicate))
			{
				map[loc] = SPACE;
				++num_removed;
				stack_removed = true;
			}
			if (!stack_removed) break;
		}
		return num_removed;
	}
}

namespace
{
	constexpr const char* TESTCASE_FOUR_INPUT = R"(..@@.@@@@.
@@@.@.@.@@
@@@@@.@.@@
@.@@@@..@.
@@.@@@@.@@
.@@@@@@@.@
.@.@.@.@@@
@.@@@.@@@@
.@@@@@@@@.
@.@.@@@.@.)";
}

ResultType testcase_four_a()
{
	std::istringstream input{ TESTCASE_FOUR_INPUT };
	return solve_p1(input);
}

ResultType testcase_four_b()
{
	std::istringstream input{ TESTCASE_FOUR_INPUT };
	return solve_p2(input);
}

ResultType advent_four_p1()
{
	auto input = advent::open_puzzle_input(4);
	return solve_p1(input);
}

ResultType advent_four_p2()
{
	auto input = advent::open_puzzle_input(4);
	return solve_p2(input);
}

#undef DAY4DBG
#undef ENABLE_DAY4DBG
