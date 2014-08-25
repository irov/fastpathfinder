#	pragma once

#	include "fastpathfinder/config.h"
#	include "fastpathfinder/point.h"
#	include "fastpathfinder/map.h"
#	include "fastpathfinder/map_test_wall.h"
#	include "fastpathfinder/pathfinder_cell.h"
#	include "fastpathfinder/helper.h"

#	include <stdint.h>
#	include <memory.h>

#	include <stdex/stl_vector.h>
#	include <stdex/stl_list.h>

namespace fastpathfinder
{
	//////////////////////////////////////////////////////////////////////////
	enum EPathFinderState
	{
		EPFS_INITIALIZE,
		EPFS_BRESENHAM,
		EPFS_GREEDY,
		EPFS_HOPELESS
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::vector<point> point_array;
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::list<point> point_list;
	//////////////////////////////////////////////////////////////////////////
	template<class TestWall = map_test_wall_2>
	class pathfinder
 	{
	public:
		pathfinder()
			: m_map(nullptr)			
			, m_width(0)
			, m_height(0)
			, m_state(EPFS_INITIALIZE)
			, m_cells(nullptr)
			, m_pathFound(false)
			, m_pathFiltred(false)
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

			return true;
		}
		
	public:
		pathfinder_cell * getCell( point _point ) const
		{
			pathfinder_cell * c = m_cells[_point.y] + _point.x;

			return c;
		}

		uint32_t getCellMask( point _point ) const
		{
			map_cell * cell = m_map->getCell( _point );

			return cell->block_mask;
		}
				
	public:
		bool findPathFirst( uint32_t _fromX, uint32_t _fromY, uint32_t _toX, uint32_t _toY )
		{
			if( _fromX >= m_width || _fromY >= m_height )
			{
				return false;
			}

			if( _toX >= m_width || _toY >= m_height )
			{
				return false;
			}

			m_from = point(_fromX, _fromY);
			m_to = point(_toX, _toY);

			if( this->getCellMask( m_from ) != 0 )
			{
				return false;
			}

			if( this->getCellMask( m_to ) != 0 )
			{
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

			m_walker_true.clear();
			m_walker_false.clear();

			m_walker_true.push_back( m_to );

			m_walker_index = m_walker_true.begin();
			m_pathFound = false;
			m_pathFiltred = false;

			m_state = EPFS_BRESENHAM;

			return true;
		}

		bool findPathNext( bool & _found )
		{
			switch( m_state )
			{
			case EPFS_INITIALIZE:
				{
					_found = false;
					return true;
				}break;
			case EPFS_BRESENHAM:
				{
					if( this->walkerBresenham( m_to, m_from ) == true )
					{
						m_walker_true.push_back( m_from );

						m_path.clear();
						m_path.push_back( m_from );
						m_path.push_back( m_to );

						m_path_filter.clear();
						m_path_filter.push_back( m_from );
						m_path_filter.push_back( m_to );

						m_pathFound = true;
						m_pathFiltred = true;

						_found = true;
						return true;
					}
									
					m_state = EPFS_GREEDY;
				}break;
			case EPFS_GREEDY:
				{
					bool found = false;
					if( this->walkerGreedy( m_from, found ) == false )
					{						
						return false;
					}

					if( found == true )
					{
						_found = found;
						return true;
					}

					m_state = EPFS_HOPELESS;
				}break;
			case EPFS_HOPELESS:
				{
					_found = false;
					return true;
				}
			};
			
			return false;
		}

		void findFilter()
		{
			if( m_pathFiltred == true )
			{
				return;
			}

			m_pathFiltred = true;

			size_t size = m_path.size();

			if( size < 2 )
			{
				return;
			}

			m_path_filter.reserve(size);

			if( size == 2 )
			{
				point p0 = m_path[0];
				point p1 = m_path[1];

				m_path_filter.push_back( p0 );
				m_path_filter.push_back( p1 );

				return;
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

					if( this->testWallBresenham( p0, p1 ) == true )
					{
						continue;
					}

					p2 = p1;
					it2 = it_next;
				}

				m_path_filter.push_back( p2 );

				it = it2;
			}
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
			
	protected:
		bool walkerGreedy( point _to, bool & _found )
		{
			if( m_walker_index == m_walker_true.end() )
			{
				if( m_walker_false.empty() == true )
				{
					_found = false;
					return true;
				}

				point p = m_walker_false.back();
				m_walker_false.pop_back();

				m_walker_true.clear();
				m_walker_true.push_back( p );

				m_walker_index = m_walker_true.begin();

				//m_walker_true.swap( m_walker_false );
				//m_walker_false.clear();
				//m_walker_index = 0;
			}
			
			point test = *m_walker_index;

			if( this->walkerWave( test, _to ) == true )
			{
				_found = true;
				return true;
			}

			++m_walker_index;
		
			return false;
		}

		bool walkerBresenham( point _from, point _to )
		{
			bresenham_line br;
			s_make_bresenham_line( br, _from, _to );

			point prev = _from;
			point next = s_next_bresenham_line_point( br, _from );

			while( true, true )
			{	
				uint32_t angle = s_get_neighbour_point_angle( prev, next );
		
				uint32_t mask = this->getCellMask( next );

				if( mask != 0 )
				{
					return false;
				}
								
				if( TestWall(m_map, m_width, m_height)( prev, angle ) == true )
				{
					return false;
				}

				if( next.x == _to.x && next.y == _to.y )
				{	
					break;
				}

				prev = next;
				next = s_next_bresenham_line_point( br, next );
			}

			return true;
		}

		bool walkerPoint( point _p, uint32_t _cost, uint32_t _angle, point & _neighbour )
		{
			int32_t dx = cell_angle_to_deltha_x[_angle];
			int32_t dy = cell_angle_to_deltha_y[_angle];

			point neighbour(_p.x + dx, _p.y + dy);

			if( neighbour.x >= m_width || neighbour.y >= m_height )
			{
				return false;
			}

			uint32_t mask = this->getCellMask( neighbour );				

			if( mask != 0 )
			{
				return false;
			}

			uint32_t next_cost = cell_angle_to_cost[_angle];
			uint32_t test_cell_cost = _cost + next_cost;

			pathfinder_cell * neighbour_cell = this->getCell( neighbour );
			uint32_t neighbour_cell_cost = neighbour_cell->cost;

			if( neighbour_cell_cost <= test_cell_cost )
			{
				return false;
			}

			if( TestWall(m_map, m_width, m_height)( _p, _angle ) == true )
			{
				return false;
			}

			neighbour_cell->cost = test_cell_cost;

			_neighbour = neighbour;

			return true;
		}

		bool walkerWave( point _p, point _to )
		{
			pathfinder_cell * cell = this->getCell( _p );

			uint32_t angle = s_get_point_angle( _p, _to );
			
			uint32_t cost = cell->cost;

			point neighbour0;
			if( walkerPoint( _p, cost, angle, neighbour0 ) == true )
			{
				if( neighbour0.x == _to.x && neighbour0.y == _to.y )
				{	
					return true;
				}

				m_walker_true.push_back( neighbour0 );
			}

			uint32_t best_angle_count = cell_best_angle_count[angle];

			for( uint32_t i = 0; i != best_angle_count; ++i )
			{
				int32_t deltha_angle = cell_next_point_deltha[i];

				uint32_t test_angle = (angle + 8 + deltha_angle) % 8;

				point neighbour;
				if( walkerPoint( _p, cost, test_angle, neighbour ) == true )
				{
					if( neighbour.x == _to.x && neighbour.y == _to.y )
					{	
						return true;
					}

					m_walker_true.push_back( neighbour );
				}
			}

			for( uint32_t i = best_angle_count; i != 7; ++i )
			{
				int32_t deltha_angle = cell_next_point_deltha[i];

				uint32_t test_angle = (angle + 8 + deltha_angle) % 8;

				point neighbour;
				if( walkerPoint( _p, cost, test_angle, neighbour ) == true )
				{
					if( neighbour.x == _to.x && neighbour.y == _to.y )
					{	
						return true;
					}

					m_walker_false.push_back( neighbour );
				}
			}
			
			return false;
		}

	public:
		bool findProcces()
		{
			if( m_pathFound == true )
			{
				return true;
			}

			m_pathFound = true;

			m_path.clear();
			
			uint32_t lx = m_to.x > m_from.x ? m_to.x - m_from.x : m_from.x > m_to.x;
			uint32_t ly = m_to.y > m_from.y ? m_to.y - m_from.y : m_from.y > m_to.y;

			m_path.reserve( lx + ly );

			m_path.push_back( m_from );

			while( true, true )
			{	
				point next = m_path.back();

				if( next.x == m_to.x && next.y == m_to.y )
				{
					return true;
				}

				if( this->findAround( next ) == false )
				{
					return false;
				}
			}

			return false;
		}

	protected:
		bool findAround( point _point )
		{
			pathfinder_cell * point_cell = this->getCell( _point );
			uint32_t cost = point_cell->cost;

			point p = _point;

			bool successful = false;
			for( uint32_t i = 0; i != 8; ++i )
			{
				int32_t dx = cell_angle_to_deltha_x[i];
				int32_t dy = cell_angle_to_deltha_y[i];

				point next;
				next.x = _point.x + dx;
				next.y = _point.y + dy;

				if( next.x >= m_width || next.y >= m_height )
				{
					continue;
				}

				pathfinder_cell * next_cell = this->getCell( next );

				if( cost < next_cell->cost )
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

		bool testWallBresenham( point _from, point _to ) const
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

		uint32_t m_width;
		uint32_t m_height;

		point m_from;
		point m_to;
		
		EPathFinderState m_state;
		
		pathfinder_cell ** m_cells;

		point_list m_walker_true;
		point_list m_walker_false;
		point_list::iterator m_walker_index;
		
		point_array m_path;
		point_array m_path_filter;

		bool m_pathFound;
		bool m_pathFiltred;
	};
}