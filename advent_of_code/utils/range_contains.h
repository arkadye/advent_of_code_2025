#pragma once

#include "advent/advent_assert.h"
#include <utility>

namespace utils
{
	template <typename T, typename U, typename V> 
		requires std::totally_ordered_with<T,U>
			&& std::totally_ordered_with<T,V>
			&& std::totally_ordered_with<U,V>
	inline bool range_contains_inc(const T& val, const U& min, const V& max)
	{
		if (min > max)
		{
			return range_contains_inc(val, max, min);
		}
		return min <= val && val <= max;
	}

	template <typename T, typename U, typename V>
		requires std::totally_ordered_with<T, U>
			&& std::totally_ordered_with<T, V>
			&& std::totally_ordered_with<U, V>
	inline bool range_contains_inc(const T& val, const std::pair<U, V>& range)
	{
		return range_contains_inc(val, range.first, range.second);
	}

	template <typename T, typename U, typename V>
		requires std::totally_ordered_with<T, U>
			&& std::totally_ordered_with<T, V>
			&& std::totally_ordered_with<U, V>
	inline bool range_contains_exc(const T& val, const U& min, const V& max)
	{
		AdventCheck(min <= max);
		return min <= val && val < max;
	}

	template <typename T, typename U, typename V>
		requires std::totally_ordered_with<T, U>
			&& std::totally_ordered_with<T, V>
			&& std::totally_ordered_with<U, V>
	inline bool range_contains_exc(const T& val, const std::pair<U, V>& range)
	{
		return range_contains_exc(val, range.first, range.second);
	}
}