#pragma once

#include <istream>
#include <string>
#include <iterator>
#include <stdexcept>
#include <optional>

namespace utils
{
	namespace istream_iterator
	{
		enum class blank_line_behaviour
		{
			skip,
			include
		};
	}

	template <istream_iterator::blank_line_behaviour blank_lines>
	class basic_istream_line_iterator
	{
	private:
		mutable std::optional<std::string> m_cached_result;
		mutable std::istream* m_stream;
		char m_sentinental;
		bool is_at_end() const { return m_stream == nullptr && !m_cached_result.has_value(); }
	public:
		using pointer = const std::string_view*;
		using reference = const std::string_view&;
		using value_type = std::string_view;
		using difference_type = int;
		using iterator_category = std::input_iterator_tag;
		explicit basic_istream_line_iterator(std::istream& stream, char sentinental = '\n') noexcept
			: m_stream{ &stream }, m_sentinental{ sentinental }
		{
			++(*this);
		}
		basic_istream_line_iterator() noexcept : m_stream{ nullptr }, m_sentinental{ 0 }{}
		basic_istream_line_iterator(const basic_istream_line_iterator&) noexcept = default;
		basic_istream_line_iterator& operator=(const basic_istream_line_iterator&) noexcept = default;
		basic_istream_line_iterator(basic_istream_line_iterator&& other) noexcept
		{
			*this = std::move(other);
		}
		basic_istream_line_iterator& operator=(basic_istream_line_iterator&& other) noexcept
		{
			this->m_stream = other.m_stream;
			other.m_stream = nullptr;
			this->m_cached_result = std::move(other.m_cached_result);
			other.m_cached_result.reset();
			this->m_sentinental = other.m_sentinental;
			return *this;
		}

		bool operator==(const basic_istream_line_iterator<blank_lines>& other) const noexcept
		{
			return this->is_at_end() && other.is_at_end();
		}

		std::string_view operator*() const
		{
			if (!m_cached_result.has_value())
			{
				throw std::range_error{ "Cannot dereference istream_line_iterator with no present value. This is probably past the end" };
			}
			return m_cached_result.value();
		}

		basic_istream_line_iterator& operator++() noexcept
		{
			if (is_at_end())
			{
				return *this;
			}

			if (m_stream == nullptr)
			{
				m_cached_result.reset();
				return *this;
			}

			std::string next_value;
			std::getline(*m_stream, next_value, m_sentinental);
			m_cached_result = std::move(next_value);

			if (m_stream->eof())
			{
				m_stream = nullptr;
			}
			return *this;
		}

		basic_istream_line_iterator  operator++(int) noexcept
		{
			auto result = *this;
			++(*this);
			return result;
		}
	};

	using istream_line_iterator = basic_istream_line_iterator<istream_iterator::blank_line_behaviour::skip>;
	using inclusive_istream_line_iterator = basic_istream_line_iterator<istream_iterator::blank_line_behaviour::include>;

	template <istream_iterator::blank_line_behaviour blank_lines>
	class basic_istream_line_range
	{
		std::istream* m_stream;
		char m_sentinental;
	public:
		explicit basic_istream_line_range(std::istream& input, char splitter = '\n') : m_stream{ &input } , m_sentinental{splitter} {}
		basic_istream_line_range() = delete;
		basic_istream_line_range(const basic_istream_line_range& other) = delete;
		basic_istream_line_range& operator=(const basic_istream_line_range& other) = delete;
		basic_istream_line_range(basic_istream_line_range&& other)
		{
			*this = std::move(other);
		}
		basic_istream_line_range& operator=(basic_istream_line_range&& other)
		{
			this->m_sentinental = other.m_sentinental;
			this->m_stream = other.m_stream;
			other.m_stream = nullptr;
			return *this;
		}
		basic_istream_line_iterator<blank_lines> begin() const { return basic_istream_line_iterator<blank_lines>{ *m_stream , m_sentinental }; }
		basic_istream_line_iterator<blank_lines> end() const { return basic_istream_line_iterator<blank_lines>{}; }
	};

	using istream_line_range = basic_istream_line_range<istream_iterator::blank_line_behaviour::skip>;
	using inclusive_istream_line_range = basic_istream_line_range<istream_iterator::blank_line_behaviour::include>;
}

template <utils::istream_iterator::blank_line_behaviour blank_lines>
inline utils::basic_istream_line_iterator<blank_lines> begin(utils::basic_istream_line_range<blank_lines> lr) { return lr.begin(); }

template <utils::istream_iterator::blank_line_behaviour blank_lines>
inline utils::basic_istream_line_iterator<blank_lines> end(utils::basic_istream_line_range<blank_lines> lr) { return lr.end(); }