#	pragma once

#	include "fastpathfinder/config.h"
#	include "fastpathfinder/point.h"
#	include "fastpathfinder/map.h"
#	include "fastpathfinder/map_test_wall.h"
#	include "fastpathfinder/pathfinder_cell.h"
#	include "fastpathfinder/helper.h"

#	include <stdint.h>
#	include <memory.h>

#	include <stdex/stl_list.h>

namespace fastpathfinder
{
	//////////////////////////////////////////////////////////////////////////
	enum EPathFinderState
	{
		EPFS_INVALID,
		EPFS_INITIALIZE,
		EPFS_STEP,
		EPFS_WALK,
		EPFS_FOUND,
		EPFS_FILTER,
		EPFS_END
	};
	//////////////////////////////////////////////////////////////////////////
	typedef FASTPATHFINDER_VECTOR<point> point_array;
	//////////////////////////////////////////////////////////////////////////
	typedef FASTPATHFINDER_LIST<point> point_list;
	//////////////////////////////////////////////////////////////////////////
	template<class TestWall = map_test_wall_2>
	class pathfinder
 	{
	public:
		pathfinder()
			: m_map(nullptr)			
			, m_width(0)
			, m_height(0)
			, m_state(EPFS_INVALID)
			, m_cells(nullptr)
			, m_win_cost((uint32_t)-1)
			, m_pathFound(false)
		{
		}

		~pathfinder()
		{
			for( uint32_t i = 0; i != m_height; ++i )
			{
				FASTPATHFINDER_FREE(m_cells[i], sizeof(pathfinder_cell) * m_width);
			}

			FASTPATHFINDER_FREE(m_cells, sizeof(pathfinder_cell *) * m_height);
		}

	public:
		bool initialize( const map * _map )
		{
			m_map = _map;

			m_width = m_map->getWidth();
			m_height = m_map->getHeight();

			m_cells = (pathfinder_cell **)FASTPATHFINDER_MALLOC(sizeof(pathfinder_cell *) * m_height);

			for( uint32_t i = 0; i != m_height; ++i )
			{
				m_cells[i] = (pathfinder_cell *)FASTPATHFINDER_MALLOC(sizeof(pathfinder_cell) * m_width);
			}

			m_state = EPFS_INITIALIZE;

			return true;
		}
		
	public:
		pathfinder_cell * getCell( point _point ) const
		{
			pathfinder_cell * c = m_cells[_point.y] + _point.x;

			return c;
		}

		map_cell * getCellMap( point _point ) const
		{
			map_cell * cell = m_map->getCell( _point );

			return cell;
		}

		uint8_t getCellMask( point _point ) const
		{
			map_cell * cell = m_map->getCell( _point );

			uint8_t mask = cell->block_mask;

			return mask;
		}
				
	public:
		bool findPathFirst( uint16_t _fromX, uint16_t _fromY, uint16_t _toX, uint16_t _toY )
		{
			if( _fromX >= m_width || _fromY >= m_height )
			{
				m_state = EPFS_INVALID;

				return false;
			}

			if( _toX >= m_width || _toY >= m_height )
			{
				m_state = EPFS_INVALID;

				return false;
			}

			if( _fromX == _toX && _fromY == _toY )
			{
				m_state = EPFS_INVALID;

				return false;
			}

			m_from = point(_fromX, _fromY);
			m_to = point(_toX, _toY);

			if( this->getCellMask( m_from ) != 0 )
			{
				m_state = EPFS_INVALID;

				return false;
			}

			if( this->getCellMask( m_to ) != 0 )
			{
				m_state = EPFS_INVALID;

				return false;
			}
						
			for( uint32_t j = 0; j != m_height; ++j )
			{
				for( uint32_t i = 0; i != m_width; ++i )
				{
					m_cells[j][i].cost = (uint32_t)-1; 
				}
			}
			
			pathfinder_cell * toCell = this->getCell( m_to );

			toCell->cost = 0;

			m_win_cost = (uint32_t)-1;

			m_walker_black.clear();
			m_walker_white.clear();

			m_walker_black.push_back( m_to );

			this->walkerBresenham( m_to, m_from );

			m_pathFound = false;

			m_state = EPFS_STEP;

			return true;
		}

	public:
		bool walkerBresenham( point _from, point _to )
		{
			bresenham_line br;
			s_make_bresenham_line( br, _from, _to );

			point prev = _from;
			point next = s_next_bresenham_line_point( br, _from );

			for(;;)
			{	
				uint32_t angle = s_get_neighbour_point_angle( prev, next );

				map_cell * map = this->getCellMap( next );

				if( map->block_mask != 0 )
				{
					return false;
				}

				if( TestWall(m_map, m_width, m_height)( prev, angle ) == true )
				{
					return false;
				}

				pathfinder_cell * prev_cell = this->getCell( prev );
				pathfinder_cell * next_cell = this->getCell( next );

				uint32_t next_cost = cell_angle_to_cost[angle];

				uint32_t next_weight_cost = next_cost * map->block_weight;

				uint32_t np_cost = prev_cell->cost + next_weight_cost;

				next_cell->cost = np_cost;

				m_walker_black.push_back( next );

				if( next.x == _to.x && next.y == _to.y )
				{	
					m_win_cost = np_cost;

					break;
				}

				prev = next;
				next = s_next_bresenham_line_point( br, next );								
			}

			return true;
		}

		bool walkerWave( bool & _found )
		{
			if( m_state != EPFS_STEP )
			{
				return false;
			}

			for( point_list::const_iterator 
				it = m_walker_black.begin(),
				it_end = m_walker_black.end();
			it != it_end;
			++it )
			{
				point p = *it;

				pathfinder_cell * p_cell = this->getCell( p );
				
				for( uint32_t i = 0; i != 8; ++i )
				{
					int16_t dx = cell_angle_to_deltha_x[i];
					int16_t dy = cell_angle_to_deltha_y[i];

					uint16_t px = p.x + dx;
					uint16_t py = p.y + dy;

					if( px >= m_width || py >= m_height )
					{
						continue;
					}

					point np(px, py);
					map_cell * np_map_cell = this->getCellMap( np );

					if( np_map_cell->block_mask != 0 )
					{
						continue;
					}

					pathfinder_cell * np_cell = this->getCell( np );

					uint32_t next_cost = cell_angle_to_cost[i];
						
					uint32_t next_weight_cost = next_cost * np_map_cell->block_weight;

					uint32_t np_cost = p_cell->cost + next_weight_cost;
					
					if( np_cost >= np_cell->cost )
					{
						continue;
					}

					if( np_cost >= m_win_cost )
					{
						continue;
					}

					
					uint16_t future_distance = s_point_distance( m_from, np );
					uint32_t future_cost = future_distance * cell_line_cost;
					uint32_t np_future_cost = p_cell->cost + future_cost;

					if( np_future_cost >= m_win_cost )
					{
						continue;
					}

					np_cell->cost = np_cost;

					if( np.x == m_from.x && np.y == m_from.y )
					{
						m_win_cost = np_cost;

						continue;
					}

					m_walker_white.push_back( np );
				}
			}

			if( m_walker_white.empty() == true )
			{
				if( m_win_cost != (uint32_t)-1 )
				{
					_found = true;
				}
				else
				{
					_found = false;
				}

				m_state = EPFS_WALK;

				return true;
			}

			m_walker_black.swap( m_walker_white );
			m_walker_white.clear();

			return false;			
		}

	public:
		const point_array & getPath() const
		{
			return m_path;
		}

		const point_array & getPathFilter() const
		{
			return m_path_filter;
		}
			
	public:
		bool findPath()
		{
			if( m_state != EPFS_WALK )
			{
				return false;
			}

			m_state = EPFS_FOUND;

			m_path.clear();
					
			uint16_t lx = m_to.x > m_from.x ? m_to.x - m_from.x : m_from.x > m_to.x;
			uint16_t ly = m_to.y > m_from.y ? m_to.y - m_from.y : m_from.y > m_to.y;

			m_path.reserve( lx + ly );

			m_path.push_back( m_from );

			for(;;)
			{	
				point next = m_path.back();

				if( next.x == m_to.x && next.y == m_to.y )
				{
					return true;
				}

				if( this->findAround_( next ) == false )
				{
					m_state = EPFS_INVALID;

					return false;
				}
			}

			m_state = EPFS_INVALID;

			return false;
		}

		bool filterPath()
		{
			if( m_state != EPFS_FOUND )
			{
				return false;
			}

			m_state = EPFS_FILTER;

			size_t size = m_path.size();

			if( size < 2 )
			{
				m_state = EPFS_INVALID;

				return false;
			}

			if( size == 2 )
			{
				point p0 = m_path[0];
				point p1 = m_path[1];

				m_path_filter.push_back( p0 );
				m_path_filter.push_back( p1 );

				return true;
			}

			point p0 = m_path[0];
			m_path_filter.clear();
			m_path_filter.push_back( p0 );

			for( size_t 
				it = 0,
				it_end = size - 2;
			it < it_end; 
			)
			{
				point p0 = m_path[it];

				size_t it2 = it + 1;
				point p2 = m_path[it2];

				for( size_t 
					it_next = it + 2,
					it_next_end = size; 
				it_next != it_next_end;
				++it_next )
				{
					point p1 = m_path[it_next];

					if( this->testWallBresenham_( p0, p1 ) == true )
					{
						continue;
					}

					p2 = p1;
					it2 = it_next;
				}

				m_path_filter.push_back( p2 );

				it = it2;
			}

			return true;
		}

	protected:
		bool findAround_( point _point )
		{
			pathfinder_cell * point_cell = this->getCell( _point );
			uint32_t cost = point_cell->cost;

			point p = _point;

			bool successful = false;
			for( uint32_t i = 0; i != 8; ++i )
			{
				int16_t dx = cell_angle_to_deltha_x[i];
				int16_t dy = cell_angle_to_deltha_y[i];

				point next;
				next.x = _point.x + dx;
				next.y = _point.y + dy;

				if( next.x >= m_width || next.y >= m_height )
				{
					continue;
				}

				pathfinder_cell * next_cell = this->getCell( next );

				if( cost <= next_cell->cost )
				{
					continue;
				}
				
				uint32_t angle = s_get_neighbour_point_angle( _point, next );

				if( TestWall(m_map, m_width, m_height)( _point, angle ) == true )
				{
					continue;
				}

				p = next;
				cost = next_cell->cost;
				successful = true;
			}

			if( successful == false )
			{
				return false;
			}

			m_path.push_back( p );

			return true;
		}

		bool testWallBresenham_( point _from, point _to ) const
		{
			bresenham_line br;
			s_make_bresenham_line( br, _from, _to );

			point next = _from;
			while( next.x != _to.x || next.y != _to.y )
			{
				point prev = next;
				next = s_next_bresenham_line_point( br, prev );

				uint32_t angle = s_get_neighbour_point_angle( prev, next );

				uint32_t mask = this->getCellMask( next );

				if( ( mask == 0 ) &&
					( TestWall(m_map, m_width, m_height)( prev, angle ) == false ) )
				{
					continue;
				}

				return true;
			}

			return false;
		}
		
	public:
		const map * m_map;

		uint16_t m_width;
		uint16_t m_height;

		point m_from;
		point m_to;

		uint32_t m_win_cost;
		
		EPathFinderState m_state;
		
		pathfinder_cell ** m_cells;

		point_list m_walker_black;
		point_list m_walker_white;
	
		point_array m_path;
		point_array m_path_filter;

		bool m_pathFound;
	};
}