#include "advent8.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY8DBG
#define DAY8DBG 1
#else
#define ENABLE_DAY8DBG 1
#ifdef NDEBUG
#define DAY8DBG 0
#else
#define DAY8DBG ENABLE_DAY8DBG
#endif
#endif

#if DAY8DBG
	#include <iostream>
#endif

namespace
{
#if DAY8DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "utils/coords3d.h"
#include "utils/parse_utils.h"
#include "utils/to_value.h"
#include "utils/istream_line_iterator.h"
#include "utils/sorted_vector.h"
#include "utils/int_range.h"
#include "utils/swap_remove.h"

#include <vector>

namespace
{
	// Easy flag to switch ambiguous behaviour.
	constexpr bool SAME_CIRCUIT_LINKS_COUNT = true;

	using CoordType = int;
	using Junction = utils::coords3d<CoordType>;
	using JunctionId = uint16_t;
	using Circuit = utils::sorted_vector<JunctionId>;
	using DistanceType = uint64_t;

	class Link
	{
	private:
		DistanceType m_distance = 0u;
		JunctionId m_low = 0u;
		JunctionId m_high = 0u;
	public:
		Link(DistanceType distance, JunctionId low, JunctionId high) : m_distance{ distance }, m_low{ low }, m_high{ high }
		{
			AdventCheck(low < high);
		}
		JunctionId get_low() const { return m_low; }
		JunctionId get_high() const { return m_high; }
		DistanceType get_distance() const { return m_distance; }
	};

	CoordType parse_coord(std::string_view elem)
	{
		return utils::to_value<CoordType>(elem);
	}

	Junction parse_junction(std::string_view line)
	{
		const auto [x, y, z] = utils::get_string_elements(line, ',', 0, 1, 2);
		return Junction{ parse_coord(x),parse_coord(y), parse_coord(z) };
	}

	std::vector<Junction> parse_junctions(std::istream& input)
	{
		std::vector<Junction> result;
		result.reserve(1000);
		stdr::transform(utils::istream_line_range{ input }, std::back_inserter(result), parse_junction);
		return result;
	}

	std::vector<Link> get_all_links(const std::vector<Junction>& junctions)
	{
		AdventCheck(junctions.size() <= std::numeric_limits<JunctionId>::max());
		std::vector<Link> result;
		result.reserve(junctions.size() * junctions.size());
		for (JunctionId low_id : utils::int_range(static_cast<JunctionId>(junctions.size())))
		{
			const auto low_junction = junctions[low_id].cast<DistanceType>();
			for (JunctionId high_id : utils::int_range(static_cast<JunctionId>(low_id + 1), static_cast<JunctionId>(junctions.size())))
			{
				const auto high_junction = junctions[high_id].cast<DistanceType>();
				const auto distance = utils::distance(low_junction, high_junction);
				result.emplace_back(distance, low_id, high_id);
			}
		}
		stdr::sort(result, {}, &Link::get_distance);
		return result;
	}

	std::vector<Link> get_all_links(std::istream& input)
	{
		const auto junctions = parse_junctions(input);
		return get_all_links(junctions);
	}

	bool add_link(std::vector<Circuit>& circuits, Link link)
	{
		auto get_current_circuit = [&circuits](JunctionId id)
			{
				return stdr::find_if(circuits, [id](const Circuit& c) {return c.contains(id); });
			};

		auto low_it = get_current_circuit(link.get_low());
		auto high_it = get_current_circuit(link.get_high());

		const bool found_low = low_it != end(circuits);
		const bool found_high = high_it != end(circuits);

		if (found_low && found_high)
		{
			if (low_it == high_it)
			{
				return false;
			}
			stdr::copy(*high_it, std::back_inserter(*low_it));
			utils::swap_remove(circuits, high_it);
		}
		else if (found_low && !found_high)
		{
			low_it->insert_keep_sorted(link.get_high());
		}
		else if (!found_low && found_high)
		{
			high_it->insert_keep_sorted(link.get_low());
		}
		else if (!found_low && !found_high)
		{
			circuits.emplace_back(Circuit{ link.get_low(), link.get_high() });
		}
		return true;
	}

	bool check_no_overlaps(const std::vector<Circuit>& circuits)
	{
		for (auto out_it = begin(circuits); out_it != end(circuits); ++out_it)
		{
			const auto& outer = *out_it;
			for (auto in_it = out_it + 1; in_it != end(circuits); ++in_it)
			{
				const auto& inner = *in_it;
				for (JunctionId id : outer)
				{
					if (inner.contains(id))
					{
						return false;
					}
				}
			}
		}
		return true;
	}

	std::vector<Circuit> build_multijunction_circuits(std::vector<Link> links_by_distance, int num_links)
	{
		std::vector<Circuit> result;
		int links_made = 0;
		for (Link link : links_by_distance)
		{
			if (add_link(result, link) || SAME_CIRCUIT_LINKS_COUNT)
			{
				++links_made;
				if (links_made == num_links)
				{
					break;
				}
			}
		}
		AdventCheck(links_made <= num_links);
		AdventCheck(check_no_overlaps(result));
		return result;
	}

	uint64_t get_circuit_product(const auto& circuits_sorted_by_size)
	{
		return stdr::fold_left(circuits_sorted_by_size | std::views::transform(&Circuit::size)
			, std::size_t{ 1 }, std::multiplies<std::size_t>{});
	}

	uint64_t solve_p1(std::istream& input, int num_links)
	{
		constexpr int NUM_CIRCUITS_TO_MULTIPLY = 3;
		const std::vector<Link> links = get_all_links(input);
		std::vector<Circuit> circuits = build_multijunction_circuits(links, num_links);
		stdr::nth_element(circuits, begin(circuits) + NUM_CIRCUITS_TO_MULTIPLY, std::greater<std::size_t>{}, & Circuit::size);
		return get_circuit_product(circuits | std::views::take(3));
	}
}

namespace
{
	int64_t solve_p2(std::istream& input)
	{
		const std::vector<Junction> junctions = parse_junctions(input);
		const std::vector<Link> links = get_all_links(junctions);
		std::vector<Circuit> circuits;

		for (Link link : links)
		{
			add_link(circuits, link);
			if (circuits.size() == 1u && circuits.front().size() == junctions.size())
			{
				const Junction low = junctions[link.get_low()];
				const Junction high = junctions[link.get_high()];

				return low.x * high.x;
			}
		}
		
		AdventUnreachable();
		return 0;
	}
}

namespace
{
	constexpr const char* TESTCASE =
		R"(162,817,812
57,618,57
906,360,560
592,479,940
352,342,300
466,668,158
542,29,236
431,825,988
739,650,466
52,470,668
216,146,977
819,987,18
117,168,530
805,96,715
346,949,466
970,615,88
941,993,340
862,61,35
984,92,344
425,690,689)";
}

ResultType testcase_eight_a()
{
	std::istringstream input{ TESTCASE };
	return solve_p1(input, 10);
}

ResultType testcase_eight_b()
{
	std::istringstream input{ TESTCASE };
	return solve_p2(input);
}

ResultType advent_eight_p1()
{
	auto input = advent::open_puzzle_input(8);
	return solve_p1(input, 1000);
}

ResultType advent_eight_p2()
{
	auto input = advent::open_puzzle_input(8);
	return solve_p2(input);
}

#undef DAY8DBG
#undef ENABLE_DAY8DBG
