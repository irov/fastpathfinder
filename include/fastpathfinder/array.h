#	pragma once

namespace fastpathfinder
{
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	struct array
	{
	public:
		typedef T value_type;

	public:
		array()
			: m_buffer(nullptr)
			, m_size(0)
			, m_current(0)
		{
		}

		~array()
		{
			delete [] m_buffer;
		}

	public:
		void initialize( size_t _size )
		{
			m_buffer = new value_type[_size];
			m_size = _size;

			m_current = 0;
		}

	public:
		void clear()
		{
			m_current = 0;
		}

		void add( value_type _value )
		{
			m_buffer[m_current] = _value;
			++m_current;
		}

		value_type pop()
		{
			--m_current;

			return m_buffer[m_current];
		}

		value_type back() const
		{
			return m_buffer[m_current - 1];
		}

		value_type * buffer() const
		{
			return m_buffer;
		}

		size_t size() const
		{
			return m_current;
		}

		value_type & operator [] ( size_t _index )
		{
			return m_buffer[_index];
		}

		value_type operator [] ( size_t _index ) const
		{
			return m_buffer[_index];
		}

		void swap( array & _other )
		{
			value_type * other_array = _other.m_buffer;
			size_t other_size = _other.m_size;
			size_t other_current = _other.m_current;

			_other.m_buffer = m_buffer;
			_other.m_size = m_size;
			_other.m_current = m_current;

			m_buffer = other_array;
			m_size = other_size;
			m_current = other_current;
		}

	protected:
		value_type * m_buffer;
		size_t m_size;

		size_t m_current;
	};
}