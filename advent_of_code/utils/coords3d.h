#pragma once

#include <type_traits>
#include <iostream>

namespace utils
{
	template <std::integral CoordsType>
	struct coords3d
	{
		CoordsType x;
		CoordsType y;
		CoordsType z;
		constexpr coords3d(CoordsType i_x, CoordsType i_y, CoordsType i_z) noexcept
			: x{ i_x }, y{ i_y }, z{ i_z } {}
		constexpr explicit coords3d(CoordsType init) noexcept : coords3d{ init,init,init } {}
		constexpr coords3d() noexcept : coords3d{ CoordsType{0} } {}
		constexpr coords3d(const coords3d&) noexcept = default;
		template <typename T> requires std::is_convertible_v<T,CoordsType>
		constexpr coords3d& operator=(const coords3d<T>& other) noexcept
		{
			x = other.x;
			y = other.y;
			z = other.z;
			return *this;
		}
		constexpr coords3d& operator+=(const coords3d& c) noexcept
		{
			x += c.x;
			y += c.y;
			z += c.z;
			return *this;
		}
		constexpr auto operator<=>(const coords3d& other) const noexcept
		{
			auto to_tuple = [](const coords3d& c)
				{
					return std::tuple{ c.z,c.x,c.y };
				};
			return to_tuple(*this) <=> to_tuple(other);
		}
		constexpr bool operator==(const coords3d&) const noexcept = default;

		constexpr CoordsType length_squared() const noexcept
		{
			return x * x + y * y + z * z;
		}

		template <std::integral NewType>
		coords3d<NewType> cast() const
		{
			if constexpr (std::is_same_v<CoordsType, NewType>)
			{
				return *this;
			}
			return coords3d<NewType>(static_cast<NewType>(x), static_cast<NewType>(y), static_cast<NewType>(z));
		}
	};

	template <typename LType, typename RType>
	auto operator+(const coords3d<LType>& l, const coords3d<RType>& r) noexcept
	{
		using ResultType = decltype(l.x + r.x);
		return coords3d<ResultType>{l.x + r.x, l.y + r.y, l.z + r.z};
	}

	template <typename LType, typename RType>
	auto operator/(const coords3d<LType>& l, RType r) noexcept
	{
		using ResultType = decltype(l.x / r);
		return coords3d<ResultType>{l.x / r, l.y / r, l.z / r};
	}

	template <typename CoordType>
	std::ostream& operator<<(std::ostream& oss, const coords3d<CoordType>& coords)
	{
		oss << coords.x << "," << coords.y << "," << coords.z;
		return oss;
	}

	template <typename LType, typename RType>
	auto distance(const coords3d<LType>& l, const coords3d<RType>& r)
	{
		auto x = std::max(l.x, r.x) - std::min(l.x, r.x);
		auto y = std::max(l.y, r.y) - std::min(l.y, r.y);
		auto z = std::max(l.z, r.z) - std::min(l.z, r.z);
		return x * x + y * y + z * z;
	}
}