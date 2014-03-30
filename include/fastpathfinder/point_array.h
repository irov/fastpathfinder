#	pragma once

#	include "fastpathfinder/point.h"

namespace fastpathfinder
{
	//////////////////////////////////////////////////////////////////////////	
	struct point_array
	{
	public:
		point_array()
			: m_array(nullptr)
			, m_size(0)
			, m_current(0)
		{
		}

		~point_array()
		{
			delete [] m_array;
		}

	public:
		void initialize( size_t _size )
		{
			m_array = new point[_size];
			m_size = _size;

			m_current = 0;
		}

	public:
		void clear()
		{
			m_current = 0;
		}

		void add( point _point )
		{
			m_array[m_current] = _point;
			++m_current;
		}

		void pop()
		{
			--m_current;
		}

		point back() const
		{
			return m_array[m_current - 1];
		}

		size_t size() const
		{
			return m_current;
		}

		void swap( point_array & _other )
		{
			point * other_array = _other.m_array;
			size_t other_size = _other.m_size;
			size_t other_current = _other.m_current;

			_other.m_array = m_array;
			_other.m_size = m_size;
			_other.m_current = m_current;

			m_array = other_array;
			m_size = other_size;
			m_current = other_current;
		}

	protected:
		point * m_array;
		size_t m_size;

		size_t m_current;
	};
}