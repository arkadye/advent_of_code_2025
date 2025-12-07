#pragma once

#include "small_vector.h"
#include <cstdint>
#include <concepts>
#include <ranges>
#include <cassert>
#include <algorithm>
#include <functional>
#include <string>

namespace utils
{
	template <std::size_t  STACK_ALLOCATION>
	class dynamic_bits
	{
	private:
		small_vector<uint64_t, (STACK_ALLOCATION + 63) / 64> m_data;
		std::size_t m_size = 0u;
		std::pair<std::size_t, std::size_t> get_sub_index_and_bitmask(std::size_t idx) const;
		void clean_end();
	public:
		dynamic_bits() = default;
		dynamic_bits(const dynamic_bits&) = default;
		dynamic_bits(dynamic_bits&& other)
		{
			operator=(std::move(other));
		}
		~dynamic_bits() = default;
		dynamic_bits& operator=(const dynamic_bits&) = default;
		dynamic_bits& operator=(dynamic_bits&& other)
		{
			m_data = std::move(other.m_data);
			m_size = other.m_size;
			other.m_size = 0u;
			return *this;
		}

		explicit dynamic_bits(std::ranges::range auto&& input);

		template <std::input_iterator ItType>
		dynamic_bits(ItType first, ItType last);

		void set_bit(std::size_t idx, bool val);
		bool get_bit(std::size_t idx) const;
		void push_back(bool);
		void pop_back();
		void resize(std::size_t new_size);

		std::size_t size() const { return m_size; }
		bool empty() const { return m_size == 0u; }
		std::size_t popcount() const;

		template <std::size_t OTHER_ALLOC>
		dynamic_bits& operator&=(const dynamic_bits<OTHER_ALLOC>& other);
		template <std::size_t OTHER_ALLOC>
		dynamic_bits& operator|=(const dynamic_bits<OTHER_ALLOC>& other);
		template <std::size_t OTHER_ALLOC>
		dynamic_bits& operator^=(const dynamic_bits<OTHER_ALLOC>& other);
		dynamic_bits operator~() const;
		dynamic_bits operator<<(std::unsigned_integral auto amount) const;
		dynamic_bits operator>>(std::unsigned_integral auto amount) const;
		dynamic_bits operator<<(std::signed_integral auto amount) const;
		dynamic_bits operator>>(std::signed_integral auto amount) const;

		template <typename...Args>
		void assign(Args&&... args) { *this = utils::dynamic_bits{ std::forward<Args>(args)... }; }

		void reserve(std::size_t new_size) { m_data.reserve((new_size + 63) / 64); }

		std::string to_string(char high, char low = ' ') const;
	};
}

template<std::size_t STACK_ALLOCATION>
inline utils::dynamic_bits<STACK_ALLOCATION>::dynamic_bits(std::ranges::range auto&& input)
{
	reserve(std::ranges::distance(input));
	for (bool val : input)
	{
		push_back(val);
	}
}

template<std::size_t STACK_ALLOCATION>
template<std::input_iterator ItType>
inline utils::dynamic_bits<STACK_ALLOCATION>::dynamic_bits(ItType first, ItType last)
 : utils::dynamic_bits{ std::ranges::subrange(first, last) }
	{}

template<std::size_t STACK_ALLOCATION>
inline void utils::dynamic_bits<STACK_ALLOCATION>::set_bit(std::size_t idx, bool val)
{
	const auto [data_idx, mask] = get_sub_index_and_bitmask(idx);
	
	uint64_t& bit = m_data[data_idx];
	if (val)
	{
		bit = bit | mask;
	}
	else
	{
		bit = bit & ~mask;
	}
}

template<std::size_t STACK_ALLOCATION>
inline bool utils::dynamic_bits<STACK_ALLOCATION>::get_bit(std::size_t idx) const
{
	const auto [data_idx, mask] = get_sub_index_and_bitmask(idx);
	return (m_data[data_idx] & mask) != 0u;
}

template<std::size_t STACK_ALLOCATION>
inline std::pair<std::size_t, uint64_t> utils::dynamic_bits<STACK_ALLOCATION>::get_sub_index_and_bitmask(std::size_t idx) const
{
	assert(idx < m_size);
	const std::size_t data_idx = idx / 64;
	assert(data_idx < m_data.size());
	const std::size_t bit_idx = idx % 64;

	// We actually want bit 0 to be HIGH bit (the leftmost one) so bitshifting works as we expect.
	const uint64_t mask = uint64_t{ 1u } << (63 - bit_idx);
	return { data_idx, mask };
}

template<std::size_t STACK_ALLOCATION>
inline void utils::dynamic_bits<STACK_ALLOCATION>::push_back(bool new_val)
{
	const std::size_t idx = m_size;
	if (m_size % 64 == 0u)
	{
		m_data.push_back(0u);
	}

	++m_size;
	if (new_val)
	{
		set_bit(idx, true);
	}
}

template<std::size_t STACK_ALLOCATION>
inline void utils::dynamic_bits<STACK_ALLOCATION>::pop_back()
{
	assert(!empty());
	if (m_size % 64 == 1u)
	{
		m_data.pop_back();
		return;
	}

	set_bit(m_size - 1, false);
	--m_size;
}

template<std::size_t STACK_ALLOCATION>
inline std::size_t utils::dynamic_bits<STACK_ALLOCATION>::popcount() const
{
	auto pop = [](uint64_t elem)
		{
			return std::popcount(elem);
		};

	return std::ranges::fold_left(m_data | std::views::transform(pop), std::size_t{ 0u }, std::plus<std::size_t>{});
}

template<std::size_t STACK_ALLOCATION>
template <std::size_t OTHER_ALLOC>
inline utils::dynamic_bits<STACK_ALLOCATION>& utils::dynamic_bits<STACK_ALLOCATION>::operator&=(const utils::dynamic_bits<OTHER_ALLOC>& other)
{
	if (other.m_data.size() > m_data.size())
	{
		m_data.resize(other.m_data.size(), 0u);
	}

	for (std::size_t i = 0u; i < std::min(m_data.size(), other.m_data.size()); ++i)
	{
		m_data[i] &= other.m_data[i];
	}
	return *this;
}

template<std::size_t STACK_ALLOCATION>
template <std::size_t OTHER_ALLOC>
inline utils::dynamic_bits<STACK_ALLOCATION>& utils::dynamic_bits<STACK_ALLOCATION>::operator|=(const utils::dynamic_bits<OTHER_ALLOC>& other)
{
	if (other.m_data.size() > m_data.size())
	{
		m_data.resize(other.m_data.size(), 0u);
	}

	for (std::size_t i = 0u; i < std::min(m_data.size(), other.m_data.size()); ++i)
	{
		m_data[i] |= other.m_data[i];
	}
	return *this;
}

template<std::size_t STACK_ALLOCATION>
template <std::size_t OTHER_ALLOC>
inline utils::dynamic_bits<STACK_ALLOCATION>& utils::dynamic_bits<STACK_ALLOCATION>::operator^=(const utils::dynamic_bits<OTHER_ALLOC>& other)
{
	if (other.m_data.size() > m_data.size())
	{
		m_data.resize(other.m_data.size(), 0u);
	}

	for (std::size_t i = 0u; i < std::min(m_data.size(), other.m_data.size()); ++i)
	{
		m_data[i] ^= other.m_data[i];
	}
	return *this;
}

template <std::size_t ALLOC_LEFT, std::size_t ALLOC_RIGHT>
inline utils::dynamic_bits<std::max(ALLOC_LEFT, ALLOC_RIGHT)> operator&(const utils::dynamic_bits<ALLOC_LEFT>& left, const utils::dynamic_bits<ALLOC_RIGHT>& right)
{
	if constexpr (ALLOC_LEFT < ALLOC_RIGHT)
	{
		return right & left;
	}

	auto result = left;
	result &= right;
	return result;
}

template <std::size_t ALLOC_LEFT, std::size_t ALLOC_RIGHT>
inline utils::dynamic_bits<std::max(ALLOC_LEFT, ALLOC_RIGHT)> operator|(const utils::dynamic_bits<ALLOC_LEFT>& left, const utils::dynamic_bits<ALLOC_RIGHT>& right)
{
	if constexpr (ALLOC_LEFT < ALLOC_RIGHT)
	{
		return right & left;
	}

	auto result = left;
	result |= right;
	return result;
}

template <std::size_t ALLOC_LEFT, std::size_t ALLOC_RIGHT>
inline utils::dynamic_bits<std::max(ALLOC_LEFT, ALLOC_RIGHT)> operator^(const utils::dynamic_bits<ALLOC_LEFT>& left, const utils::dynamic_bits<ALLOC_RIGHT>& right)
{
	if constexpr (ALLOC_LEFT < ALLOC_RIGHT)
	{
		return right & left;
	}

	auto result = left;
	result ^= right;
	return result;
}

template<std::size_t STACK_ALLOCATION>
inline void utils::dynamic_bits<STACK_ALLOCATION>::resize(std::size_t new_size)
{
	const std::size_t internal_size = (new_size + 63) / 64;
	m_data.resize(internal_size);
	m_size = new_size;
}

template <std::size_t STACK_ALLOCATION>
inline void utils::dynamic_bits<STACK_ALLOCATION>::clean_end()
{
	const std::size_t unused_at_end = m_size % 64;
	if (unused_at_end)
	{
		const uint64_t end_mask = std::numeric_limits<uint64_t>::max() << unused_at_end;
		m_data.back() &= end_mask;
	}
}

template <std::size_t STACK_ALLOCATION>
inline  utils::dynamic_bits<STACK_ALLOCATION> utils::dynamic_bits<STACK_ALLOCATION>::operator~() const
{
	utils::dynamic_bits<STACK_ALLOCATION> result;
	result.reserve(size());

	std::ranges::transform(m_data, std::back_inserter(result.m_data), std::bit_not<uint64_t>{});

	result.clean_end();
	return result;
}

template<std::size_t STACK_ALLOCATION>
inline utils::dynamic_bits<STACK_ALLOCATION> utils::dynamic_bits<STACK_ALLOCATION>::operator<<(std::unsigned_integral auto amount) const
{
	utils::dynamic_bits<STACK_ALLOCATION> result;
	if (amount == 0u)
	{
		result = *this;
		return result;
	}

	result.m_data.resize(m_data.size(), 0u);
	result.m_size = m_size;
	if (amount > size())
	{
		return result;
	}

	auto elem_offset = amount / 64;
	auto bit_offset = amount % 64;

	uint64_t carry = 0u;

	for (auto i = std::ssize(m_data)-1; i >= 0; --i)
	{
		const auto target_idx = i - elem_offset;
		if (target_idx < 0) break;
		
		const uint64_t from_elem = m_data[i];
		result.m_data[target_idx] = (from_elem << bit_offset) | carry;

		carry = from_elem >> (64 - bit_offset);
	}
	result.clean_end();
	return result;
}

template<std::size_t STACK_ALLOCATION>
inline utils::dynamic_bits<STACK_ALLOCATION> utils::dynamic_bits<STACK_ALLOCATION>::operator>>(std::unsigned_integral auto amount) const
{
	utils::dynamic_bits<STACK_ALLOCATION> result;
	if (amount == 0u)
	{
		result = *this;
		return result;
	}

	result.m_data.resize(m_data.size(), 0u);
	result.m_size = m_size;
	if (amount > size())
	{
		return result;
	}

	auto elem_offset = amount / 64;
	auto bit_offset = amount % 64;

	uint64_t carry = 0u;

	for (int64_t i = 0; i < std::ssize(m_data); ++i)
	{
		const auto target_idx = i + elem_offset;
		if (target_idx >= std::ssize(result.m_data)) break;

		const uint64_t from_elem = m_data[i];
		result.m_data[target_idx] = (from_elem >> bit_offset) | carry;

		carry = from_elem << (64 - bit_offset);
	}
	result.clean_end();
	return result;
}

template<std::size_t STACK_ALLOCATION>
inline utils::dynamic_bits<STACK_ALLOCATION> utils::dynamic_bits<STACK_ALLOCATION>::operator<<(std::signed_integral auto amount) const
{
	if (amount < 0)
	{
		return this->operator>>(-amount);
	}

	using SignedType = decltype(amount);
	using UnsignedType = std::make_unsigned_t<SignedType>;
	const auto as_unsigned = static_cast<UnsignedType>(amount);
	return this->operator<<(as_unsigned);
}

template<std::size_t STACK_ALLOCATION>
inline utils::dynamic_bits<STACK_ALLOCATION> utils::dynamic_bits<STACK_ALLOCATION>::operator>>(std::signed_integral auto amount) const
{
	if (amount < 0)
	{
		return this->operator<<(-amount);
	}

	using SignedType = decltype(amount);
	using UnsignedType = std::make_unsigned_t<SignedType>;
	const auto as_unsigned = static_cast<UnsignedType>(amount);
	return this->operator>>(as_unsigned);
}

template<std::size_t STACK_ALLOCATION>
inline std::string utils::dynamic_bits<STACK_ALLOCATION>::to_string(char high, char low) const
{
	std::string result;
	result.reserve(size());

	for (std::size_t i = 0u; i < size(); ++i)
	{
		result.push_back(get_bit(i) ? high : low);
	}

	return result;
}