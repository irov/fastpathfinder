#	pragma once

#	include "fastpathfinder/config.h"
#	include "fastpathfinder/point.h"
#	include "fastpathfinder/map_cell.h"

#	include <memory.h>
#	include <new>

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
			for( uint32_t i = 0; i != m_height; ++i )
			{
				FASTPATHFINDER_FREE(m_cells[i], sizeof(map_cell) * m_width);
			}

			FASTPATHFINDER_FREE(m_cells, sizeof(map_cell *) * m_height);
		}

	public:
		bool initialize( uint16_t _width, uint16_t _height )
		{
			m_width = _width;
			m_height = _height;

			m_cells = (map_cell **)FASTPATHFINDER_MALLOC(sizeof(map_cell *) * m_height);
			
			for( uint32_t i = 0; i != m_height; ++i )
			{
				m_cells[i] = (map_cell *)FASTPATHFINDER_MALLOC(sizeof(map_cell) * m_width);
			}
			
			this->clear();

			return true;
		}

		void clear()
		{
			for( size_t j = 0; j != m_height; ++j )
			{
				for( size_t i = 0; i != m_width; ++i )
				{
					m_cells[j][i].block_mask = 0;
					m_cells[j][i].block_weight = 1;
				}
			}
		}

	public:
		uint16_t getWidth() const
		{
			return m_width;
		}

		uint16_t getHeight() const
		{
			return m_height;
		}

	public:
		bool setCellMask( uint16_t _x, uint16_t _y, uint8_t _mask )
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

		bool getCellMask( uint16_t _x, uint16_t _y, uint8_t & _mask ) const
		{
			if( _x > m_width || _y > m_height )
			{
				return false;
			}

			point p(_x, _y);
			map_cell * c = this->getCell( p );

			uint8_t mask = c->block_mask;

			_mask = mask;

			return true;
		}

	public:
		void setCellWeightBuffer( const uint8_t * _weight )
		{
			for( size_t j = 0; j != m_height; ++j )
			{
				for( size_t i = 0; i != m_width; ++i )
				{
					size_t index = i + j * m_width;

					uint8_t cell_weight = _weight[index];

					if( cell_weight == 0 )
					{
						m_cells[j][i].block_weight = 1;
					}
					else
					{
						m_cells[j][i].block_weight = cell_weight;
					}
				}
			}
		}

		bool setCellWeight( uint16_t _x, uint16_t _y, uint8_t _weight )
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

		bool getCellWeight( uint16_t _x, uint16_t _y, uint8_t & _weight ) const
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
			map_cell * c = m_cells[_point.y] + _point.x;

			return c;
		}

	protected:
		uint16_t m_width;
		uint16_t m_height;

		map_cell ** m_cells;
	};
}