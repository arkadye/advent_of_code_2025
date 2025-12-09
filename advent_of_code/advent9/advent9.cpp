#include "advent9.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY9DBG
#define DAY9DBG 1
#else
#define ENABLE_DAY9DBG 1
#ifdef NDEBUG
#define DAY9DBG 0
#else
#define DAY9DBG ENABLE_DAY9DBG
#endif
#endif

#if DAY9DBG
	#include <iostream>
#endif

namespace
{
#if DAY9DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "utils/coords.h"
#include "utils/istream_line_iterator.h"
#include "utils/comparisons.h"

#include <vector>
#include <algorithm>

namespace
{
	using Tile = utils::coords64;

	std::vector<Tile> parse_tiles(std::istream& input)
	{
		std::vector<Tile> result;
		result.reserve(500);
		stdr::transform(utils::istream_line_range{ input }, std::back_inserter(result), Tile::from_chars);
		return result;
	}

	int64_t get_rectangle_size_p1(const Tile& a, const Tile& b)
	{
		const auto offset = a - b;
		auto side = [](auto x) {return std::abs(x) + 1; };
		const auto result = side(offset.x) * side(offset.y);
		return result;
	}

	// Just use the dumb n^2 solution
	int64_t solve_p1(std::istream& input)
	{
		const std::vector<Tile> point_cache = parse_tiles(input);
		int64_t biggest_rectangle = 0u;

		for (auto lower_it = begin(point_cache); lower_it != end(point_cache); ++lower_it)
		{
			const Tile& tile = *lower_it;
			auto rect = [&tile](const Tile& other) { return get_rectangle_size_p1(tile, other); };
			auto biggest = [](int64_t a, int64_t b) { return std::max(a, b); };

			biggest_rectangle = stdr::fold_left(stdr::transform_view(stdr::subrange(std::next(lower_it), end(point_cache)), rect), biggest_rectangle, biggest);
		}

		return biggest_rectangle;
	}
}

#include <utility>

#include "utils/range_contains.h"

namespace
{
	class Subline
	{
	private:
		int64_t m_low{};
		int64_t m_high{};
	public:
		Subline(int64_t from, int64_t to) : m_low{ std::min(from,to) }, m_high{ std::max(from,to) } {}
		int64_t low() const { return m_low; }
		int64_t high() const { return m_high; }

		bool intersects(const Subline& other) const
		{
			if (low() > other.high()) return false;
			if (other.low() > high()) return false;
			return true;
		}
	};
	
	// A lookup for lines running in the direction given in the template.
	template <utils::direction DIRECTION>
	class ParallelLines
	{
		// std::flat_map isn't in my C++ implementation yet :-(
		using ValueType = std::pair<int64_t, Subline>; // Major-axis location and line on that location
		using DataType = std::vector<ValueType>;
		DataType m_data;

		auto lower_bound(int64_t val) const
		{
			AdventCheck(stdr::is_sorted(m_data, {}, &ValueType::first));
			return stdr::lower_bound(m_data, val, {}, &ValueType::first);
		}

		auto upper_bound(int64_t val) const
		{
			AdventCheck(stdr::is_sorted(m_data, {}, &ValueType::first));
			return stdr::upper_bound(m_data, val, {}, &ValueType::first);
		}

		void add_line(int64_t major_axis, const Subline& from_to)
		{
			m_data.emplace_back(major_axis, from_to);
		}

		static int64_t get_major(const Tile& tile)
		{
			if constexpr (utils::is_horizontal(DIRECTION))
			{
				return tile.y;
			}
			if constexpr (utils::is_vertical(DIRECTION))
			{
				return tile.x;
			}
		}

		static int64_t get_minor(const Tile& tile)
		{
			if constexpr (utils::is_horizontal(DIRECTION))
			{
				return tile.x;
			}
			if constexpr (utils::is_vertical(DIRECTION))
			{
				return tile.y;
			}
		}
		static Subline get_minor_line(const Tile& from, const Tile& to)
		{
			AdventCheck(get_major(from) == get_major(to));
			return Subline{ get_minor(from), get_minor(to) };
		}

		static std::pair<Subline, Subline> get_major_minor_ranges(const Tile& a, const Tile& b)
		{
			const Subline major{ get_major(a) , get_major(b) };
			const Subline minor{ get_minor(a), get_minor(b) };
			return { major, minor };
		}

	public:
		void add_line(const Tile& from, const Tile& to)
		{
			if (get_major(from) == get_major(to))
			{
				add_line(get_major(from), get_minor_line(from, to));
			}
		}

		void finished_adding_lines()
		{
			std::sort(begin(m_data), end(m_data), [](const ValueType& l, const ValueType& r) {return l.first < r.first; });
			//stdr::sort(m_data, utils::Smaller<int64_t>{}, &ValueType::first);
		}

		bool intersects(const Tile& corner, const Tile& other_corner) const
		{
			// Get the ends of the range, ignoring lines on the actual edges.
			auto remove_border = [](const Subline sl) { return Subline{ sl.low() + 1, sl.high() - 1 }; };

			auto [major_axis, minor_axis] = get_major_minor_ranges(corner, other_corner);
			major_axis = remove_border(major_axis);
			minor_axis = remove_border(minor_axis);
			const auto low_it = lower_bound(major_axis.low());
			const auto high_it = upper_bound(major_axis.high());

			auto elem_intersects = [&minor_axis](const Subline& other)
				{
					return minor_axis.intersects(other);
				};
			return stdr::any_of(stdr::subrange(low_it, high_it), elem_intersects, &DataType::value_type::second);
		}

		bool is_inside(const Tile& tile) const
		{
			const auto minor_axis = get_minor(tile);
			const auto major_axis = get_major(tile);
			const auto search_range = [this, major_axis]()
				{
					const auto from_left_it = upper_bound(major_axis);
					const auto from_right_it = lower_bound(major_axis);

					const auto left_range = stdr::subrange(begin(m_data), from_left_it);
					const auto right_range = stdr::subrange(from_right_it, end(m_data));

					const auto left_dist = stdr::distance(left_range);
					const auto right_dist = stdr::distance(right_range);

					return left_dist < right_dist ? left_range : right_range;
				}();

			// Count the number of lines crossing from the edge to this point. Odd numbers are inside, even numbers are outside!

			auto num_crossings = [minor_axis](const Subline& line)
				{
					return utils::range_contains_inc(minor_axis, line.low(), line.high());
				};
			const auto result = stdr::count_if(search_range, num_crossings, &ValueType::second);
			const bool inside = (result % 2) == 1;
			return inside;
		}
	};

	class Lines
	{
		ParallelLines<utils::direction::right> x_parallel_lines;
		ParallelLines<utils::direction::up> y_parallel_lines;
	public:
		void add_line(const Tile& from, const Tile& to)
		{
			x_parallel_lines.add_line(from, to);
			y_parallel_lines.add_line(from, to);
		}

		void finish_adding_lines()
		{
			x_parallel_lines.finished_adding_lines();
			y_parallel_lines.finished_adding_lines();
		}

		bool intersects(const Tile& from, const Tile& to) const
		{
			return x_parallel_lines.intersects(from, to) || y_parallel_lines.intersects(from, to);
		}

		bool is_inside(const Tile& tile) const
		{
			return x_parallel_lines.is_inside(tile) && y_parallel_lines.is_inside(tile);
		}
	};

	Lines get_lines(const std::vector<Tile>& tiles)
	{
		Lines result;
		for (const auto& [from, to] : tiles | std::views::adjacent<2>)
		{
			result.add_line(from, to);
		}
		if (tiles.size() > 1u)
		{
			result.add_line(tiles.back(), tiles.front());
		}
		result.finish_adding_lines();
		return result;
	}

	int64_t get_rectangle_size_p2(int64_t threshold, const Tile& a, const Tile& b, const Lines& lines)
	{
		const int64_t result = get_rectangle_size_p1(a, b);
		if (result <= threshold) return threshold;

		const Tile middle = (a + b) / 2;
		if (!lines.is_inside(middle))
		{
			return threshold;
		}

		if (lines.intersects(a, b))
		{
			return threshold;
		}

		return result;
	}

	int64_t solve_p2(std::istream& input)
	{
		const std::vector<Tile> tiles = parse_tiles(input);
		const Lines lines = get_lines(tiles);

		int64_t result = 0;

		for (auto lower_it = begin(tiles); lower_it != end(tiles); ++lower_it)
		{
			const Tile& lower = *lower_it;
			for (auto higher_it = std::next(lower_it); higher_it != end(tiles); ++higher_it)
			{
				const Tile& higher = *higher_it;
				result = get_rectangle_size_p2(result, lower, higher, lines);
			}
		}

		return result;
	}
}

namespace
{
	constexpr const char* TESTCASE =
R"(7,1
11,1
11,7
9,7
9,5
2,5
2,3
7,3)";
}

ResultType testcase_nine_a()
{
	std::istringstream input{ TESTCASE };
	return solve_p1(input);
}

ResultType testcase_nine_b()
{
	std::istringstream input{ TESTCASE };
	return solve_p2(input);
}

ResultType advent_nine_p1()
{
	auto input = advent::open_puzzle_input(9);
	return solve_p1(input);
}

ResultType advent_nine_p2()
{
	auto input = advent::open_puzzle_input(9);
	return solve_p2(input);
}

#undef DAY9DBG
#undef ENABLE_DAY9DBG
