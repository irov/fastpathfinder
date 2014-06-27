#	pragma once

#	include "fastpathfinder/point.h"
#	include "fastpathfinder/map_cell.h"

#	include <memory.h>

namespace fastpathfinder
{
	//////////////////////////////////////////////////////////////////////////
	class map
	{
	public:
		map()
			: m_width(0)
			, m_height(0)
			, m_cells(nullptr)
		{
		}

		~map()
		{
			delete [] m_cells;
		}

	public:
		bool initialize( uint32_t _width, uint32_t _height )
		{
			m_width = _width;
			m_height = _height;

			m_cells = new map_cell[m_width * m_height];

			return true;
		}

		void clear()
		{
			memset( m_cells, 0, m_width * m_height * sizeof(m_cells[0]) );
		}

	public:
		uint32_t getWidth() const
		{
			return m_width;
		}

		uint32_t getHeight() const
		{
			return m_height;
		}

	public:
		bool setCellMask( uint32_t _x, uint32_t _y, uint32_t _mask )
		{
			if( _x >= m_width || _y >= m_height )
			{
				return false;
			}

			point p(_x, _y);
			map_cell * c = this->getCell( p );

			c->block_mask = _mask;

			return true;
		}

		bool getCellMask( uint32_t _x, uint32_t _y, uint32_t & _mask ) const
		{
			if( _x > m_width || _y > m_height )
			{
				return false;
			}

			point p(_x, _y);
			map_cell * c = this->getCell( p );

			uint32_t mask = c->block_mask;

			_mask = mask;

			return true;
		}

	public:
		bool setCellWeight( uint32_t _x, uint32_t _y, uint8_t _weight )
		{
			if( _x >= m_width || _y >= m_height )
			{
				return false;
			}

			point p(_x, _y);
			map_cell * c = this->getCell( p );

			c->block_weight = _weight;

			return true;
		}

		bool getCellWeight( uint32_t _x, uint32_t _y, uint8_t & _weight ) const
		{
			if( _x > m_width || _y > m_height )
			{
				return false;
			}

			point p(_x, _y);
			map_cell * c = this->getCell( p );

			uint8_t weight = c->block_weight;

			_weight = weight; 

			return true;
		}

	public:
		map_cell * getCell( point _point ) const
		{
			uint32_t index = _point.x + _point.y * m_width;
			map_cell * c = m_cells + index;

			return c;
		}

	protected:
		size_t m_width;
		size_t m_height;
		map_cell * m_cells;
	};
}