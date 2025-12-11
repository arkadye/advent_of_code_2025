#include "advent11.h"
#include "advent/advent_utils.h"

#ifdef FORCE_DAY11DBG
#define DAY11DBG 1
#else
#define ENABLE_DAY11DBG 1
#ifdef NDEBUG
#define DAY11DBG 0
#else
#define DAY11DBG ENABLE_DAY11DBG
#endif
#endif

#if DAY11DBG
	#include <iostream>
#endif

namespace
{
#if DAY11DBG
	std::ostream& log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "utils/small_vector.h"
#include "utils/split_string.h"
#include "utils/string_line_iterator.h"
#include "utils/istream_line_iterator.h"
#include "utils/int_range.h"

#include <map>

namespace
{
	using Node = uint32_t;
	using OutputList = utils::small_vector<Node, 4>;
	using Device = std::pair<const Node, OutputList>;
	using Graph = std::map<Node, OutputList>;
	static_assert(std::is_same_v<Device, Graph::value_type>);

	Node parse_node(std::string_view label)
	{
		AdventCheck(label.size() <= 4u);
		Node result = 0u;
		std::memcpy(&result, label.data(), label.size());
		return result;
	}

	Device parse_device(std::string_view line)
	{
		auto [label, output_labels_str] = utils::split_string_at_first(line, ':');
		label = utils::trim_string(label);
		output_labels_str = utils::trim_string(output_labels_str);
		const Node node = parse_node(label);
		const auto output_range = utils::string_line_range{ output_labels_str, ' '};
		const auto num_outputs = stdr::count(output_labels_str, ' ') + 1;
		OutputList outputs;
		outputs.reserve(num_outputs);
		stdr::transform(output_range, std::back_inserter(outputs), parse_node);
		return std::pair{ node, std::move(outputs) };
	}

	Graph parse_graph(std::istream& input)
	{
		Graph result;
		stdr::transform(utils::istream_line_range{ input }, std::inserter(result,result.begin()), parse_device);
		return result;
	}

	using Memo = std::map<Node, int64_t>;

	int64_t get_num_paths_impl(Memo& memo, const Graph& graph, Node from, Node to, const utils::small_vector<Node, 2>& forbidden_targets)
	{
		// First check if we are at out destination.
		if (from == to) return 1;

		// Check if we can find this in the memo. Use lower_bound so we have
		// an insert hint to use later, if necessary.
		const auto memo_find_result = memo.lower_bound(from);
		if (memo_find_result != end(memo) && memo_find_result->first == from)
		{
			return memo_find_result->second;
		}

		// It wasn't in the memo, so find the list of next nodes in the graph.
		const auto device_it = graph.find(from);
		if (device_it == end(graph)) return 0;

		// Add up all the routes.
		auto impl = [&memo, &graph, from, to, &forbidden_targets](Node next_from) -> int64_t
			{
				if (next_from != to && stdr::contains(forbidden_targets, next_from))
				{
					return 0;
				}
				return get_num_paths_impl(memo, graph, next_from, to, forbidden_targets);
			};

		const auto result = stdr::fold_left(device_it->second | std::views::transform(impl), int64_t{0}, std::plus<int64_t>{});
		
		// Add the result to the memo.
		memo.insert(memo_find_result, std::pair{ from, result });
		return result;
	}

	int64_t get_num_paths_impl(const Graph& graph, Node from, Node to)
	{
		AdventCheck(graph.contains(from));
		Memo memo;
		return get_num_paths_impl(memo, graph, from, to, {});
	}

	int64_t get_num_paths(std::istream& graph_stream, std::string_view from_label, std::string_view to_label, utils::small_vector<std::string_view, 2> extra_target_labels = {})
	{
		const Graph graph = parse_graph(graph_stream);
		const Node from = parse_node(from_label);
		const Node to = parse_node(to_label);
		if (extra_target_labels.empty())
		{
			return get_num_paths_impl(graph, from, to);
		}

		utils::small_vector<Node, 2> extra_targets;
		stdr::transform(extra_target_labels, std::back_inserter(extra_targets), parse_node);
		stdr::sort(extra_targets);

		std::map<Node, Memo> memos_per_target;

		auto get_steps = [&memos_per_target, &graph, &extra_targets](std::tuple<Node,Node> path)
			{
				const Node from = std::get<0>(path);
				const Node to = std::get<1>(path);
				Memo& memo = memos_per_target[to];
				return get_num_paths_impl(memo, graph, from, to, extra_targets);
			};

		int64_t total = 0u;

		// Each permutation of extra_targets represents a possible route.
		do
		{
			const int64_t first_step = get_steps({ from, extra_targets.front() });
			const int64_t last_step = get_steps({ extra_targets.back(), to });
			const int64_t subtotal = stdr::fold_left(extra_targets | std::views::adjacent<2> | stdr::views::transform(get_steps), first_step * last_step, std::multiplies<int64_t>{});
			total += subtotal;

		} while (stdr::next_permutation(extra_targets).found);
		return total;
	}

	int64_t solve_p1(std::istream& input)
	{
		return get_num_paths(input, "you", "out");
	}

	int64_t solve_p2(std::istream& input)
	{
		return get_num_paths(input, "svr", "out", { "dac" , "fft" });
	}
}

namespace
{
	constexpr const char* TESTCASE_A =
R"(aaa: you hhh
you: bbb ccc
bbb: ddd eee
ccc: ddd eee fff
ddd: ggg
eee: out
fff: out
ggg: out
hhh: ccc fff iii
iii: out)";

	constexpr const char* TESTCASE_B =
R"(svr: aaa bbb
aaa: fft
fft: ccc
bbb: tty
tty: ccc
ccc: ddd eee
ddd: hub
hub: fff
eee: dac
dac: fff
fff: ggg hhh
ggg: out
hhh: out)";
}
ResultType testcase_eleven_a()
{
	std::istringstream input{ TESTCASE_A };
	return solve_p1(input);
}

ResultType testcase_eleven_b()
{
	std::istringstream input{ TESTCASE_B };
	return solve_p2(input);
}

ResultType advent_eleven_p1()
{
	auto input = advent::open_puzzle_input(11);
	return solve_p1(input);
}

ResultType advent_eleven_p2()
{
	auto input = advent::open_puzzle_input(11);
	return solve_p2(input);
}

#undef DAY11DBG
#undef ENABLE_DAY11DBG
