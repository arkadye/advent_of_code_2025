#pragma once

#include <cstdint>
#include <limits>

namespace utils
{
	uint32_t hash_combine(uint32_t old_hash, uint32_t new_hash)
	{
		// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0814r0.pdf
		const uint32_t result = new_hash + 0x9e3779b9 + (old_hash << 6) + (new_hash >> 2);
		return result;
	}

	uint64_t hash_combine(uint64_t old_hash, uint64_t new_hash)
	{
		auto lo = [](uint64_t in)
			{
				constexpr uint64_t mask = std::numeric_limits<uint32_t>::max();
				const uint64_t result = in & mask;
				return static_cast<uint32_t>(result);
			};

		auto hi = [](uint64_t in)
			{
				const uint64_t result = in >> 32;
				return static_cast<uint32_t>(result);
			};

		const uint64_t low_hash = hash_combine(lo(old_hash), hi(new_hash));
		const uint64_t high_hash = hash_combine(hi(old_hash), lo(new_hash));
		return (high_hash << 32) | low_hash;
	}
}