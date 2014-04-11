#	pragma once

#	include <stdint.h>
#	include <memory.h>

#	include "fastpathfinder/point.h"
#	include "fastpathfinder/array.h"
#	include "fastpathfinder/map.h"
#	include "fastpathfinder/map_test_wall.h"
#	include "fastpathfinder/pathfinder_cell.h"
#	include "fastpathfinder/helper.h"

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
	template<class TestWall = map_test_wall_2>
	class pathfinder
	{
	public:
		typedef array<point> point_array;

	public:
		pathfinder()
			: m_map(nullptr)			
			, m_width(0)
			, m_height(0)
			, m_revision(0)
			, m_state(EPFS_INITIALIZE)
			, m_walker_index(0)
			, m_cells(nullptr)
		{
		}

		~pathfinder()
		{
			delete [] m_cells;
		}

	public:
		bool initialize( const map * _map )
		{
			m_map = _map;

			m_width = m_map->getWidth();
			m_height = m_map->getHeight();

			uint32_t sq_map = m_width * m_height;

			m_cells = new pathfinder_cell[sq_map];
			
			m_walker_true.initialize( sq_map * 2);
			m_walker_false.initialize( sq_map * 2);
			m_path.initialize( sq_map );
			m_path_filter.initialize( sq_map );

			return true;
		}
		
	public:
		uint32_t getRevision() const
		{
			return m_revision;
		}

	public:
		pathfinder_cell * getCell( point _point ) const
		{
			uint32_t index = _point.x + _point.y * m_width;
			pathfinder_cell * c = m_cells + index;

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

			++m_revision;

			pathfinder_cell * toCell = this->getCell( m_to );

			toCell->revision = m_revision;
			toCell->cost = 0;

			m_walker_true.clear();
			m_walker_false.clear();

			m_walker_true.add( m_from );

			m_walker_index = 0;

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
					if( this->walkerBresenham( m_from, m_to ) == true )
					{
						_found = true;
						return true;
					}

					m_walker_index = m_walker_true.size();
										
					m_state = EPFS_GREEDY;
				}break;
			case EPFS_GREEDY:
				{
					bool found;
					if( this->walkerGreedy( m_to, m_from, found ) == false )
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
			size_t size = m_path.size();

			if( size < 2 )
			{
				return;
			}

			if( size == 2 )
			{
				point p0 = m_path[0];
				point p1 = m_path[1];

				m_path_filter.add( p0 );
				m_path_filter.add( p1 );

				return;
			}

			point p0 = m_path[0];
			m_path_filter.clear();
			m_path_filter.add( p0 );

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

				m_path_filter.add( p2 );

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
		bool walkerGreedy( point _from, point _to, bool & _found )
		{
			size_t it_walker = m_walker_true.size();
			
			if( m_walker_true.size() == 0 )
			{
				_found = false;
				return true;
			}
			
			size_t size = m_walker_true.size();

			if( m_walker_index > size )
			{
				m_walker_index = size;
			}

			point next = m_walker_true[m_walker_index - 1];

			if( this->walkerBresenham( next, _to ) == true )
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

			pathfinder_cell * from_cell = this->getCell( _from );

			uint32_t cost = from_cell->cost;

			point prev = _from;
			point next = s_next_bresenham_line_point( br, _from );

			while( true, true )
			{	
				uint32_t angle = s_get_neighbour_point_angle( prev, next );

				uint32_t next_cost = s_get_next_point_cost( prev, next );
				uint32_t test_cell_cost = cost + next_cost;

				pathfinder_cell * next_cell = this->getCell( next );
				uint32_t next_cell_cost = next_cell->cost;

				bool tw = TestWall(m_map, m_width, m_height)( prev, angle );

				uint32_t mask = this->getCellMask( next );

				if( ( mask != 0 ) || 
					( next_cell->revision == m_revision && next_cell_cost <= test_cell_cost ) ||
					( tw == true ) )
				{
					uint32_t best_index = cell_best_angle_count[angle];

					for( uint32_t i = 0; i != best_index; ++i )
					{
						if( this->nextWalkTrue( prev, cost, angle, i ) == true )
						{
							return false;
						}
					}

					m_walker_true.pop();

					return false;
				}

				next_cell->revision = m_revision;
				next_cell->cost = test_cell_cost;

				m_walker_true.add( next );

				if( next.x == _to.x && next.y == _to.y )
				{	
					break;
				}

				cost = test_cell_cost;

				prev = next;
				next = s_next_bresenham_line_point( br, next );
			}

			return true;
		}

		bool findProcces( point _from, point _to )
		{
			m_path.clear();
			m_path.add( _from );

			while( true, true )
			{	
				point next = m_path.back();

				if( next.x == _to.x && next.y == _to.y )
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

		bool findAround( point _point )
		{
			pathfinder_cell * point_cell = this->getCell( _point );
			uint32_t cost = point_cell->cost;

			point p = _point;

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

				if( next_cell->revision != m_revision )
				{
					continue;
				}

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
			}

			m_path.add( p );

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

				cell * c = this->getCell( next );

				if( ( c->block_mask == 0 ) &&
					( TestWall(m_cells, m_width, m_height)( prev, angle ) == false ) )
				{
					continue;
				}

				return true;
			}

			return false;
		}

		bool nextWalkTrue( point _prev, uint32_t _cost, uint32_t _angle, uint32_t _index )
		{
			uint32_t next_angle = s_get_next_foreach_angle( _angle, _index );
			point nearest = s_get_point_next_angle( _prev, next_angle );

			if( nearest.x >= m_width || nearest.y >= m_height )
			{
				return false;
			}

			uint32_t nearest_cost = s_get_next_point_cost( _prev, nearest );
			uint32_t setup_cell_cost = _cost + nearest_cost;
			uint32_t test_cell_cost = setup_cell_cost + cell_line_cost * 2;
			
			uint32_t mask = this->getCellMask( nearest );

			if( mask != 0 )
			{
				return false;
			}

			pathfinder_cell * nearest_cell = this->getCell( nearest );

			if( nearest_cell->revision == m_revision && nearest_cell->cost <= test_cell_cost )
			{
				return false;
			}

			if( TestWall(m_map, m_width, m_height)( _prev, next_angle ) == true )
			{
				return false;
			}

			nearest_cell->revision = m_revision;
			nearest_cell->cost = setup_cell_cost;

			m_walker_true.add( nearest );

			return true;
		}

		void nextWalkFalse( point _prev, uint32_t _weight, uint32_t _angle, uint32_t _index )
		{
			uint32_t next_angle = s_get_next_foreach_angle( _angle, _index );
			point nearest = s_get_point_next_angle( _prev, next_angle );

			if( nearest.x >= m_width || nearest.y >= m_height )
			{
				return;
			}

			uint32_t mask = this->getCellMask( nearest );
			
			if( mask != 0 )
			{
				return;
			}

			pathfinder_cell * nearest_cell = this->getCell( nearest );
			
			if( nearest_cell->revision == m_revision ) 
			{
				return;
			}

			if( TestWall(m_map, m_width, m_height)( _prev, next_angle ) == true )
			{
				return;
			}

			uint32_t nearest_cost = s_get_next_point_cost( _prev, nearest );
			uint32_t setup_cell_cost = _weight + nearest_cost;
			//uint32_t test_cell_weight = setup_cell_weight /*+ cell_weight_line * 2*/;

			nearest_cell->revision = m_revision;
			nearest_cell->cost = setup_cell_cost;

			m_walker_false.add( nearest );
		}
		
	public:
		const map * m_map;

		uint32_t m_width;
		uint32_t m_height;

		point m_from;
		point m_to;
		
		uint32_t m_revision;
		EPathFinderState m_state;
		uint32_t m_walker_index;

		pathfinder_cell * m_cells;

		point_array m_walker_true;
		point_array m_walker_false;
		point_array m_path;
		point_array m_path_filter;
	};
}