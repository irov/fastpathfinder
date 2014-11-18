#	pragma once

#	include <stdint.h>

#	include "fastpathfinder/point.h"
#	include "fastpathfinder/map.h"
#	include "fastpathfinder/helper.h"

namespace fastpathfinder
{
	//////////////////////////////////////////////////////////////////////////
	class map_test_wall_2
	{
	public:
		map_test_wall_2( const map * _map, uint16_t _width, uint16_t _height )
			: m_map(_map)
			, m_width(_width)
			, m_height(_height)
		{
		}

	public:
		bool operator() ( point _point, uint32_t _angle ) const
		{
			bool test_wall = cell_angle_to_test_wall[_angle];

			if( test_wall == false )
			{
				return false;
			}

			for( uint32_t i = 0; i != 2; ++i )
			{
				uint32_t next_angle = s_get_next_foreach_angle( _angle, i );
				point nearest = s_get_point_next_angle( _point, next_angle );

				if( nearest.x >= m_width || nearest.y >= m_height )
				{
					continue;
				}

				const map_cell * nearest_cell = m_map->getCell( nearest );

				if( nearest_cell->block_mask == 0 )
				{
					continue;
				}

				return true;
			}

			return false;
		}

	protected:
		const map * m_map;
		uint16_t m_width;
		uint16_t m_height;
	};
	//////////////////////////////////////////////////////////////////////////
	class map_test_wall_1
	{
	public:
		map_test_wall_1( const map * _map, uint16_t _width, uint16_t _height )
			: m_map(_map)
			, m_width(_width)
			, m_height(_height)
		{
		}

	public:
		bool operator() ( point _point, uint16_t _angle ) const
		{
			bool test_wall = cell_angle_to_test_wall[_angle];

			if( test_wall == false )
			{
				return false;
			}

			size_t wall_count = 0;
			for( uint32_t i = 0; i != 2; ++i )
			{
				uint32_t next_angle = s_get_next_foreach_angle( _angle, i );
				point nearest = s_get_point_next_angle( _point, next_angle );

				if( nearest.x >= m_width || nearest.y >= m_height )
				{
					continue;
				}

				const map_cell * nearest_cell = m_map->getCell( nearest );

				if( nearest_cell->block_mask == 0 )
				{
					continue;
				}

				++wall_count;
			}

			if( wall_count == 2 )
			{
				return false;
			}

			return false;
		}

	protected:
		const map * m_map;
		uint32_t m_width;
		uint32_t m_height;
	};
	//////////////////////////////////////////////////////////////////////////
	class map_test_wall_none
	{
	public:
		map_test_wall_none( const map * _map, uint16_t _width, uint16_t _height )
		{
			(void)_map;
			(void)_width;
			(void)_height;
		}

	public:
		bool operator() ( point _point, uint32_t _angle ) const
		{
			(void)_point;
			(void)_angle;

			return false;
		}
	};
}