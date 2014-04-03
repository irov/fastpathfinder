#	pragma once

#	include <stdint.h>
#	include <memory.h>

#	include "fastpathfinder/cell.h"
#	include "fastpathfinder/point.h"
#	include "fastpathfinder/point_array.h"

namespace fastpathfinder
{
	//////////////////////////////////////////////////////////////////////////
	static const int32_t cell_next_point_deltha[7] = {-1, 1, -2, 2, -3, 3, 4};
	//////////////////////////////////////////////////////////////////////////
	static const int32_t cell_matrix_to_deltha_x[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
	static const int32_t cell_matrix_to_deltha_y[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
	//////////////////////////////////////////////////////////////////////////
	static const int32_t cell_angle_to_deltha_x[8] = {1, 1, 0, -1, -1, -1, 0, 1};
	static const int32_t cell_angle_to_deltha_y[8] = {0, -1, -1, -1, 0, 1, 1, 1};
	//////////////////////////////////////////////////////////////////////////
	static const uint32_t cell_weight_diagonal = 140;
	static const uint32_t cell_weight_line = 99;
	static const uint32_t cell_angle_to_weight[8] = {cell_weight_line, cell_weight_diagonal, cell_weight_line, cell_weight_diagonal, cell_weight_line, cell_weight_diagonal, cell_weight_line, cell_weight_diagonal};
	static const uint32_t cell_diagonal_to_weight[3] = {0, cell_weight_line, cell_weight_diagonal};
	//////////////////////////////////////////////////////////////////////////
	static const uint32_t cell_matrix_to_angle[9] = {3, 2, 1, 4, 8, 0, 5, 6, 7};
	static const uint32_t cell_angle_to_matrix[9] = {5, 2, 1, 0, 3, 6, 7, 8, 4};
	static const bool cell_angle_to_test_wall[9] = {false, true, false, true, false, true, false, true, false};
	//////////////////////////////////////////////////////////////////////////
	struct bresenham_line
	{
		int32_t deltaX;
		int32_t deltaY;

		int32_t signX;
		int32_t signY;

		int32_t error;
	};
	//////////////////////////////////////////////////////////////////////////
	inline static void s_make_bresenham_line( bresenham_line & _br, point _from, point _to )
	{
		_br.deltaX = _to.x > _from.x ? _to.x - _from.x : _from.x - _to.x;
		_br.deltaY = _to.y > _from.y ? _to.y - _from.y : _from.y - _to.y;

		_br.signX = _from.x < _to.x ? 1 : -1;
		_br.signY = _from.y < _to.y ? 1 : -1;

		_br.error = _br.deltaX - _br.deltaY;
	}
	//////////////////////////////////////////////////////////////////////////
	inline static point s_next_bresenham_line_point( bresenham_line & _br, point _point )
	{
		point next_point(_point);

		const int32_t error2 = _br.error * 2;

		if( error2 > -_br.deltaY )
		{
			_br.error -= _br.deltaY;

			next_point.x += _br.signX;
		}

		if( error2 < _br.deltaX )
		{
			_br.error += _br.deltaX;

			next_point.y += _br.signY;
		}

		return next_point;
	}
	//////////////////////////////////////////////////////////////////////////
	inline static uint32_t s_get_next_point_angle( point _from, point _to )
	{
		uint32_t deltaX = _to.x >= _from.x ? (_to.x - _from.x) + 1 : 0;
		uint32_t deltaY = _to.y >= _from.y ? (_to.y - _from.y) + 1 : 0;

		uint32_t matrix = deltaX + deltaY * 3;

		uint32_t angle = cell_matrix_to_angle[matrix];

		return angle;
	}
	//////////////////////////////////////////////////////////////////////////
	inline static uint32_t s_get_next_point_weight( point _from, point _to )
	{
		uint32_t deltaX = _to.x > _from.x ? _to.x - _from.x : _from.x - _to.x;
		uint32_t deltaY = _to.y > _from.y ? _to.y - _from.y : _from.y - _to.y;

		uint32_t diagonal = deltaX + deltaY;

		uint32_t weight = cell_diagonal_to_weight[diagonal];

		return weight;
	}
	//////////////////////////////////////////////////////////////////////////
	inline static uint32_t s_get_next_foreach_angle( uint32_t _angle, uint32_t _index )
	{
		int32_t deltha = cell_next_point_deltha[_index];

		uint32_t next_angle = (_angle + 8 + deltha) % 8;

		return next_angle;
	}
	//////////////////////////////////////////////////////////////////////////
	inline static point s_get_point_next_angle( point _point, uint32_t _angle )
	{
		uint32_t matrix = cell_angle_to_matrix[_angle];

		int32_t dx = cell_matrix_to_deltha_x[matrix];
		int32_t dy = cell_matrix_to_deltha_y[matrix];

		uint32_t nx = _point.x + dx;
		uint32_t ny = _point.y + dy;

		point np(nx, ny);

		return np;
	}
	//////////////////////////////////////////////////////////////////////////
	inline static uint32_t s_point_distance( point p0, point p1 )
	{
		uint32_t dx = ( p1.x > p0.x ) ? p1.x - p0.x : p0.x - p1.x;
		uint32_t dy = ( p1.y > p0.y ) ? p1.y - p0.y : p0.y - p1.y;

		uint32_t distance = ( dy > dx ) ? ( (961 * dy + 398 * dx ) / 1000 ) : ( (961 * dx + 398 * dy ) / 1000 );

		return distance;
	}
	//////////////////////////////////////////////////////////////////////////
	class map_test_wall
	{
	public:
		map_test_wall( const cell * _cells, uint32_t _width, uint32_t _height )
			: m_cells(_cells)
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

				const cell * nearest_cell = this->getCell( nearest );

				if( nearest_cell->block_mask == 0 )
				{
					continue;
				}

				return true;
			}

			return false;
		}

	protected:
		const cell * getCell( point _point ) const
		{
			uint32_t index = (_point.x + _point.y * m_width);
			const cell * c = m_cells + index;

			return c;
		}

	protected:
		const cell * m_cells;
		uint32_t m_width;
		uint32_t m_height;
	};
	//////////////////////////////////////////////////////////////////////////
	class map_test_wall_none
	{
	public:
		map_test_wall_none( const cell * _cell, uint32_t _width, uint32_t _height )
		{
			(void)_cell;
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
	//////////////////////////////////////////////////////////////////////////
	template<class TestWall = map_test_wall>
	class map
	{
	public:
		map()
			: m_width(0)
			, m_height(0)
			, m_cells(nullptr)
			, m_revision(0)
			, m_walkerWeight(0)
		{
		}

	public:
		bool initialize( uint32_t _width, uint32_t _height )
		{
			m_width = _width;
			m_height = _height;

			m_cells = new cell[m_width * m_height];
			m_walker_true.initialize( m_width * m_height * 2);
			m_walker_false.initialize( m_width * m_height * 2);
			m_path.initialize( m_width * m_height );
			m_path_filter.initialize( m_width * m_height );

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

		uint32_t getRevision() const
		{
			return m_revision;
		}

	public:
		bool setCellMask( uint32_t _x, uint32_t _y, uint32_t _mask )
		{
			if( _x > m_width || _y > m_height )
			{
				return false;
			}

			point point(_x, _y);
			cell * c = this->getCell( point );

			c->block_mask = _mask;

			return true;
		}

		uint32_t getCellMask( uint32_t _x, uint32_t _y ) const
		{
			if( _x > m_width || _y > m_height )
			{
				return false;
			}

			point point(_x, _y);
			cell * c = this->getCell( point );

			uint32_t mask = c->block_mask;

			return mask;
		}
				
	public:
		bool findPath( uint32_t _fromX, uint32_t _fromY, uint32_t _toX, uint32_t _toY )
		{
			point from(_fromX, _fromY);

			cell * fromCell = this->getCell( from );

			if( fromCell->block_mask != 0 )
			{
				return false;
			}

			point to(_toX, _toY);

			cell * toCell = this->getCell( to );

			if( toCell->block_mask != 0 )
			{
				return false;
			}

			++m_revision;

			toCell->block_revision = m_revision;
			toCell->weight = 0;

			if( this->walkerProcessFirst( to, from ) == false )
			{
				return false;
			}

			if( this->findProcces( from, to ) == false )
			{
				return false;
			}

			return true;
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
			
	public:
		cell * getCell( point _point ) const
		{
			uint32_t index = (_point.x + _point.y * m_width);
			cell * c = m_cells + index;

			return c;
		}

	protected:
		bool walkerProcessFirst( point _from, point _to )
		{
			m_walker_true.clear();
			m_walker_false.clear();

			m_walker_true.add( _from );

			if( this->walkerBresenham( _from, _to ) == true )
			{
				return true;
			}

			while( m_walker_true.size() > 0 )
			{
				while( m_walker_true.size() > 0 )
				{	
					point next = m_walker_true.back();

					if( this->walkerBresenham( next, _to ) == true )
					{
						return true;
					}
				}

				m_walker_false.swap( m_walker_true );
			}

			return false;
		}

		bool walkerBresenham( point _from, point _to )
		{
			bresenham_line br;
			s_make_bresenham_line( br, _from, _to );

			cell * from_cell = this->getCell( _from );

			uint32_t weight = from_cell->weight;

			point prev = _from;
			point next = s_next_bresenham_line_point( br, _from );

			while( true, true )
			{	
				uint32_t angle = s_get_next_point_angle( prev, next );

				uint32_t next_weight = s_get_next_point_weight( prev, next );
				uint32_t next_cell_weight = weight + next_weight;

				cell * next_cell = this->getCell( next );

				if( ( next_cell->block_mask != 0 ) || 
					( next_cell->block_revision == m_revision && next_cell->weight <= next_cell_weight ) ||
					( TestWall(m_cells, m_width, m_height)( prev, angle ) == true ) )
				{
					for( uint32_t i = 0; i != 2; ++i )
					{
						if( this->nextWalk( prev, weight, angle, i, m_walker_true ) == false )
						{
							continue;
						}

						return false;
					}

					m_walker_true.pop();

					for( uint32_t i = 2; i != 7; ++i )
					{
						if( this->nextWalk( prev, weight, angle, i, m_walker_false ) == false )
						{
							continue;
						}
					}

					return false;
				}

				if( next.x == _to.x && next.y == _to.y )
				{
					break;
				}

				next_cell->block_revision = m_revision;
				next_cell->weight = next_cell_weight;

				m_walker_true.add( next );

				weight = next_cell_weight;

				prev = next;
				next = s_next_bresenham_line_point( br, next );				
			}

			cell * next_cell = this->getCell( next );

			uint32_t next_weight = s_get_next_point_weight( prev, next );
			uint32_t next_cell_weight = weight + next_weight;

			next_cell->block_revision = m_revision;
			next_cell->weight = next_cell_weight;

			m_walker_true.add( next );

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
			cell * point_cell = this->getCell( _point );
			uint32_t weight = point_cell->weight;

			point p = _point;

			for( uint32_t i = 0; i != 8; ++i )
			{
				int32_t dx = cell_angle_to_deltha_x[i];
				int32_t dy = cell_angle_to_deltha_y[i];

				point next;
				next.x = _point.x + dx;
				next.y = _point.y + dy;

				if( next.x > m_width || next.y > m_height )
				{
					continue;
				}

				cell * next_cell = this->getCell( next );

				if( next_cell->block_revision != m_revision )
				{
					continue;
				}

				if( weight < next_cell->weight )
				{
					continue;
				}

				p = next;
				weight = next_cell->weight;				
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

				uint32_t angle = s_get_next_point_angle( prev, next );

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

		bool nextWalk( point _prev, uint32_t _weight, uint32_t _angle, uint32_t _index, point_array & _array )
		{
			uint32_t next_angle = s_get_next_foreach_angle( _angle, _index );
			point nearest = s_get_point_next_angle( _prev, next_angle );

			if( nearest.x >= m_width || nearest.y >= m_height )
			{
				return false;
			}

			uint32_t nearest_weight = s_get_next_point_weight( _prev, nearest );
			uint32_t nearest_cell_weight = _weight + nearest_weight;

			cell * nearest_cell = this->getCell( nearest );

			if( ( nearest_cell->block_mask != 0 ) || 
				( nearest_cell->block_revision == m_revision && nearest_cell->weight <= nearest_cell_weight ) ||
				( TestWall(m_cells, m_width, m_height)( _prev, next_angle ) == true ) )
			{
				return false;
			}

			nearest_cell->block_revision = m_revision;
			nearest_cell->weight = nearest_cell_weight;

			_array.add( nearest );

			return true;
		}
		
	public:
		uint32_t m_width;
		uint32_t m_height;

		cell * m_cells;
		
		uint32_t m_revision;
		uint32_t m_walkerWeight;

		point_array m_walker_true;
		point_array m_walker_false;
		point_array m_path;
		point_array m_path_filter;
	};
}