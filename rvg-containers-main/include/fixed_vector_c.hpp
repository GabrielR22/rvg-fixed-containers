#pragma once
/*
	MIT License

	Copyright(c) 2023 Gabriel Romanzini

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this softwareand associated documentation files(the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions :

	The above copyright noticeand this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

//std::initializer_list
#include <cassert>
#include <utility>

#include "internal/helper.h"
#include "internal/iterator_c.h"

namespace rvg
{
	template <typename T, size_t Size>
	class fixed_vector_c
	{
	private:

		//A Slot has the value + a bool to control if its being used.
		//using Slot = std::pair<T, bool>;

		using type = T;
		using type_ptr = T*;
		using type_ref = T&;
		using c_type_ref = T&;

		//Slot iterator
		using it_slot = rvg::internal::Iterator_c<type>;

	public:

		constexpr fixed_vector_c() = default;

		//! constructs a vector with il list.
		constexpr fixed_vector_c(std::initializer_list<type> list)
		{
			assert(list.size() && list.size() < Size);

			memcpy_s(m_Data.data(), Size * sizeof(type), list.begin(), list.size() * sizeof(decltype(*list.begin())));

			m_CurSize = list.size();
		};

		//! Gets the data inside this vector.
		//constexpr T& data() { return m_Data.data(); };

		constexpr type_ptr find(c_type_ref el_)
		{
			for (auto& el : m_Data)
			{
				if (el == el_)
					return &el;
			}

			return nullptr;
		}

		//! Creates a begin iterator
		it_slot begin() { return it_slot(m_Data.data(), &m_Data[Size - 1]); }
		//! End is just a nullptr;
		it_slot end() { return it_slot( m_CurSize == m_Data.size() ? nullptr : &m_Data[m_CurSize], &m_Data[Size - 1]); }

		//! Emplaces a value if there is an slot.
		// <success> can fail if vector is full </success>
		// <returns> a pointer to the element if emplace was valid </returns>
		constexpr T* emplace(T&& el) { return m_emplace(std::move(el)); }
		constexpr T* emplace(T& el) { return m_emplace(std::move(el)); }

		constexpr bool erase(c_type_ref el_) { return m_erase(std::move(el_)); };
		constexpr bool erase(T&& el_) { return m_erase(std::move(el_)); };

	private:

		constexpr bool m_erase(T&& el_)
		{
			if (const size_t index = m_find_slot(el_); index != rvg::internal::uint_upperbound)
			{
				const int cpy_amount = (m_CurSize-1) - index;
				assert(cpy_amount > 0);

				const auto* wh = (&m_Data[index+1]);
				memcpy(&m_Data[index],wh, cpy_amount * sizeof(type));

				memset(&m_Data[m_CurSize-1], 0, sizeof(type));

				--m_CurSize;

				return true;
			}

			return false;
		}
		constexpr T* m_emplace(T&& el)
		{
			if (m_CurSize < Size)
			{
				m_Data[m_CurSize] = el;
				m_CurSize++;
				return &el;
			}

			//if (auto* sl = m_get_first_empty())
			//{
			//	sl = el;
			//	m_CurSize++;
			//	return &sl;
			//}

			return nullptr;
		}

		constexpr size_t m_find_slot(c_type_ref el_)
		{
			for (size_t index = 0, end = m_Data.size(); index < end; ++index)
			{
				if (m_Data[index] == el_)
					return index;
			}

			return -1;
		}

		//constexpr static void m_mark_used(Slot* sl_) { (*sl_).second = true; }
		//constexpr static void m_mark_unused(Slot* sl_) { (*sl_).second = false; }

		constexpr T* m_get_first_empty()
		{
			for (auto& sl : m_Data)
			{
				if (!sl.second)
					return &sl;
			}

			return nullptr;
		}
	private:
		std::array<T, Size> m_Data{};

		size_t m_CurSize = 0;
	};



	template <class First, class... Rest>
	fixed_vector_c(First, Rest...) -> fixed_vector_c<typename internal::Enforce_same<First, Rest...>::type, 1 + sizeof...(Rest)>;
}