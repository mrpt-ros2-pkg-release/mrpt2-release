/* +------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)            |
   |                          https://www.mrpt.org/                         |
   |                                                                        |
   | Copyright (c) 2005-2019, Individual contributors, see AUTHORS file     |
   | See: https://www.mrpt.org/Authors - All rights reserved.               |
   | Released under BSD License. See: https://www.mrpt.org/License          |
   +------------------------------------------------------------------------+ */
#pragma once

#include <mrpt/core/aligned_allocator.h>  // aligned_allocator_cpp11
#include <array>
#include <cstddef>  // size_t
#include <type_traits>  // conditional_t, ...
#include <vector>

namespace mrpt::containers
{
struct UnspecializedBool
{
	operator const bool&() const { return b; }
	operator bool&() { return b; }

	bool b;
};

/** Container that transparently and dynamically switches between a std::array
 * and std::vector. Used to avoid heap allocations with small vectors.
 *
 * \note In `#include <mrpt/containers/vector_with_small_size_optimization.h>`
 * \ingroup mrpt_containers_grp
 */
template <typename VAL, size_t small_size, size_t alignment = 16>
class vector_with_small_size_optimization
{
   private:
	using T =
		std::conditional_t<std::is_same_v<VAL, bool>, UnspecializedBool, VAL>;
	using ALLOC = mrpt::aligned_allocator_cpp11<T>;
	using vector =
		vector_with_small_size_optimization<VAL, small_size, alignment>;
	using large_vec = std::vector<T, ALLOC>;
	using small_array = std::array<T, small_size>;

	large_vec m_v;
	alignas(alignment) small_array m_a;
	bool m_is_small = true;
	size_t m_size = 0;

   public:
	using value_type = T;
	using reference = T&;
	using const_reference = const T&;
	using difference_type = typename large_vec::difference_type;
	using size_type = typename large_vec::size_type;
	template <typename TYPE, typename POINTER, typename REFERENCE>
	class iteratorImpl
	{
		using STORAGE = std::conditional_t<
			std::is_same_v<POINTER, bool*>, UnspecializedBool*,
			std::conditional_t<
				std::is_same_v<POINTER, const bool*>, const UnspecializedBool*,
				POINTER>>;
		using self = iteratorImpl<TYPE, POINTER, REFERENCE>;

	   public:
		using value_type = TYPE;
		using reference = REFERENCE;
		using pointer = POINTER;
		using iterator_category = std::random_access_iterator_tag;
		using difference_type = typename large_vec::difference_type;
		iteratorImpl() = default;
		iteratorImpl(STORAGE ptr) : m_ptr(ptr) {}
		self operator++()
		{
			self i = *this;
			m_ptr++;
			return i;
		}
		self operator--()
		{
			self i = *this;
			m_ptr--;
			return i;
		}
		self operator++(int)
		{
			m_ptr++;
			return *this;
		}
		self operator--(int)
		{
			m_ptr--;
			return *this;
		}
		self operator+(difference_type n)
		{
			self i = *this;
			i.m_ptr += n;
			return i;
		}
		self operator-(difference_type n)
		{
			self i = *this;
			i.m_ptr -= n;
			return i;
		}
		self operator+=(difference_type n)
		{
			m_ptr += n;
			return *this;
		}
		self operator-=(difference_type n)
		{
			m_ptr -= n;
			return *this;
		}
		difference_type operator-(const self& o) { return m_ptr - o.m_ptr; }
		REFERENCE operator*() { return *m_ptr; }
		POINTER operator->() { return m_ptr; }
		bool operator==(const self& o) { return m_ptr == o.m_ptr; }
		bool operator!=(const self& o) { return m_ptr != o.m_ptr; }

	   private:
		STORAGE m_ptr{nullptr};
	};

	using iterator = iteratorImpl<VAL, VAL*, VAL&>;
	using const_iterator = iteratorImpl<VAL, const VAL*, const VAL&>;

	void resize(size_type n)
	{
		if (m_size)
		{
			if (m_is_small && n > small_size)
			{
				m_v.assign(m_a.begin(), m_a.begin() + m_size);
			}
			else if (!m_is_small && n <= small_size)
			{
				std::copy(m_v.begin(), m_v.begin() + n, m_a.begin());
			}
		}
		m_size = n;
		m_is_small = (n <= small_size);
		if (!m_is_small)
		{
			m_v.resize(m_size);
		}
	}

	size_t size() const { return m_size; }
	bool empty() const { return m_size == 0; }

	reference operator[](size_type n) { return m_is_small ? m_a[n] : m_v[n]; }

	const_reference operator[](size_type n) const
	{
		return m_is_small ? m_a[n] : m_v[n];
	}

	const_reference back() const
	{
		return m_is_small ? m_a[m_size - 1] : m_v.back();
	}
	reference back() { return m_is_small ? m_a[m_size - 1] : m_v.back(); }

	const_reference front() const
	{
		return m_is_small ? m_a.front() : m_v.front();
	}
	reference front() { return m_is_small ? m_a.front() : m_v.front(); }

	void swap(vector& x)
	{
		if (m_is_small & x.m_is_small)
		{
			m_a.swap(x.m_a);
		}
		else if (!m_is_small && !x.m_is_small)
		{
			m_v.swap(x.m_v);
		}
		else if (!m_is_small && x.m_is_small)
		{
			std::copy(x.m_a.begin(), x.m_a.begin() + x.m_size, m_a.begin());
			x.m_v.swap(m_v);
		}
		else
		{
			m_v.swap(x.m_v);
			std::copy(m_a.begin(), m_a.begin() + m_size, x.m_a.begin());
		}
		std::swap(m_size, x.m_size);
		std::swap(m_is_small, x.m_is_small);
	}

	iterator begin() noexcept { return m_is_small ? m_a.data() : m_v.data(); }
	const_iterator begin() const noexcept
	{
		return m_is_small ? m_a.data() : m_v.data();
	}

	iterator end() noexcept
	{
		return m_is_small ? m_a.data() + m_size : m_v.data() + m_size;
	}
	const_iterator end() const noexcept
	{
		return m_is_small ? m_a.data() + m_size : m_v.data() + m_size;
	}
};

}  // namespace mrpt::containers
