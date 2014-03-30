#	include "fastpathfinder/fastpathfinder.h"

namespace fastpathfinder
{
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
	static void s_make_bresenham_line( bresenham_line & _br, point _from, point _to )
	{
		_br.deltaX = _to.x > _from.x ? _to.x - _from.x : _from.x - _to.x;
		_br.deltaY = _to.y > _from.y ? _to.y - _from.y : _from.y - _to.y;

		_br.signX = _from.x < _to.x ? 1 : -1;
		_br.signY = _from.y < _to.y ? 1 : -1;

		_br.error = _br.deltaX - _br.deltaY;
	}
	//////////////////////////////////////////////////////////////////////////
	static point s_next_bresenham_line_point( bresenham_line & _br, point _point )
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
	const int32_t cell_next_point_deltha[7] = {-1, 1, -2, 2, -3, 3, 4};
	//////////////////////////////////////////////////////////////////////////
	const int32_t cell_matrix_to_deltha_x[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
	const int32_t cell_matrix_to_deltha_y[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
	//////////////////////////////////////////////////////////////////////////
	const int32_t cell_angle_to_deltha_x[8] = {1, 1, 0, -1, -1, -1, 0, 1};
	const int32_t cell_angle_to_deltha_y[8] = {0, -1, -1, -1, 0, 1, 1, 1};
	//////////////////////////////////////////////////////////////////////////
	const uint32_t cell_weight_diagonal = 140;
	const uint32_t cell_weight_line = 99;
	const uint32_t cell_angle_to_weight[8] = {cell_weight_line, cell_weight_diagonal, cell_weight_line, cell_weight_diagonal, cell_weight_line, cell_weight_diagonal, cell_weight_line, cell_weight_diagonal};
	const uint32_t cell_diagonal_to_weight[3] = {0, cell_weight_line, cell_weight_diagonal};
	//////////////////////////////////////////////////////////////////////////
	const uint32_t cell_matrix_to_angle[9] = {3, 2, 1, 4, 8, 0, 5, 6, 7};
	const uint32_t cell_angle_to_matrix[9] = {5, 2, 1, 0, 3, 6, 7, 8, 4};
	//////////////////////////////////////////////////////////////////////////
	static uint32_t s_get_next_point_angle( point _from, point _to )
	{
		uint32_t deltaX = _to.x >= _from.x ? (_to.x - _from.x) + 1 : 0;
		uint32_t deltaY = _to.y >= _from.y ? (_to.y - _from.y) + 1 : 0;

		uint32_t matrix = deltaX + deltaY * 3;

		uint32_t angle = cell_matrix_to_angle[matrix];

		return angle;
	}
	//////////////////////////////////////////////////////////////////////////
	static uint32_t s_get_next_point_weight( point _from, point _to )
	{
		uint32_t deltaX = _to.x > _from.x ? _to.x - _from.x : _from.x - _to.x;
		uint32_t deltaY = _to.y > _from.y ? _to.y - _from.y : _from.y - _to.y;

		uint32_t diagonal = deltaX + deltaY;

		uint32_t weight = cell_diagonal_to_weight[diagonal];

		return weight;
	}
	//////////////////////////////////////////////////////////////////////////
	static point s_get_next_foreach_point( point _point, uint32_t _angle, uint32_t _index )
	{
		int32_t deltha = cell_next_point_deltha[_index];

		uint32_t next_angle = (_angle + 8 + deltha) % 8;

		uint32_t next_matrix = cell_angle_to_matrix[next_angle];

		int32_t dx = cell_matrix_to_deltha_x[next_matrix];
		int32_t dy = cell_matrix_to_deltha_y[next_matrix];

		uint32_t nx = _point.x + dx;
		uint32_t ny = _point.y + dy;

		point np(nx, ny);

		return np;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t s_point_distance( point p0, point p1 )
	{
		uint32_t dx = ( p1.x > p0.x ) ? p1.x - p0.x : p0.x - p1.x;
		uint32_t dy = ( p1.y > p0.y ) ? p1.y - p0.y : p0.y - p1.y;
				
		uint32_t distance = ( dy > dx ) ? ( (961 * dy + 398 * dx ) / 1000 ) : ( (961 * dx + 398 * dy ) / 1000 );
		
		return distance;
	}
	//////////////////////////////////////////////////////////////////////////
	map::map()
		: m_width(0)
		, m_height(0)
		, m_cells(nullptr)
		, m_revision(0)
		, m_walkerWeight(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool map::initialize( uint32_t _width, uint32_t _height )
	{
		m_width = _width;
		m_height = _height;

		m_cells = new cell[m_width * m_height];
		m_walker_true.initialize( m_width * m_height * 2);
		m_walker_false.initialize( m_width * m_height * 2);
		m_path.initialize( m_width * m_height );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool map::setCellMask( uint32_t _x, uint32_t _y, uint32_t _mask )
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
	//////////////////////////////////////////////////////////////////////////
	uint32_t map::getCellMask( uint32_t _x, uint32_t _y ) const
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
	//////////////////////////////////////////////////////////////////////////
	cell * map::getCell( point _point ) const
	{
		uint32_t index = (_point.x + _point.y * m_width);
		cell * c = m_cells + index;

		return c;
	}
	//////////////////////////////////////////////////////////////////////////
	bool map::findPath( uint32_t _fromX, uint32_t _fromY, uint32_t _toX, uint32_t _toY )
	{
		point from(_fromX, _fromY);
		point to(_toX, _toY);

		cell * fromCell = this->getCell( from );

		if( fromCell->block_mask != 0 )
		{
			return false;
		}

		++m_revision;

		fromCell->block_revision = m_revision;
		fromCell->weight = 0;

		if( this->walkerProcessFirst( from, to ) == false )
		{
			return false;
		}
			
		if( this->findProcces( to, from ) == false )
		{
			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool map::findAround( point _point )
	{
		cell * point_cell = this->getCell( _point );
		uint32_t weight = point_cell->weight;

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

			uint32_t next_weight = cell_angle_to_weight[i];

			if( weight - next_weight != next_cell->weight )
			{
				continue;
			}

			m_path.add( next );

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool map::findProcces( point _from, point _to )
	{
		m_path.clear();
		m_path.add( _from );

		while( true )
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
	//////////////////////////////////////////////////////////////////////////
	bool map::walkerProcessFirst( point _from, point _to )
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
	//////////////////////////////////////////////////////////////////////////
	bool map::walkerBresenham( point _from, point _to )
	{
		bresenham_line br;
		s_make_bresenham_line( br, _from, _to );

		cell * from_cell = this->getCell( _from );

		uint32_t weight = from_cell->weight;

		point prev = _from;
		point next = s_next_bresenham_line_point( br, _from );

		while( next.x != _to.x || next.y != _to.y )
		{	
			uint32_t next_weight = s_get_next_point_weight( prev, next );
			uint32_t next_cell_weight = weight + next_weight;

			cell * next_cell = this->getCell( next );

			if( ( next_cell->block_mask != 0 ) || 
				( next_cell->block_revision == m_revision && next_cell->weight <= next_cell_weight ) )
			{
				uint32_t angle = s_get_next_point_angle( prev, next );

				for( uint32_t i = 0; i != 2; ++i )
				{
					point nearest = s_get_next_foreach_point( prev, angle, i );

					if( nearest.x >= m_width || nearest.y >= m_height )
					{
						continue;
					}

					uint32_t nearest_weight = s_get_next_point_weight( prev, nearest );
					uint32_t nearest_cell_weight = weight + nearest_weight;

					cell * nearest_cell = this->getCell( nearest );

					if( ( nearest_cell->block_mask != 0 ) || 
						( nearest_cell->block_revision == m_revision && nearest_cell->weight <= nearest_cell_weight ) )
					{
						continue;
					}

					nearest_cell->block_revision = m_revision;
					nearest_cell->weight = nearest_cell_weight;

					m_walker_true.add( nearest );

					return false;
				}

				m_walker_true.pop();

				for( uint32_t i = 2; i != 7; ++i )
				{
					point nearest = s_get_next_foreach_point( prev, angle, i );

					if( nearest.x >= m_width || nearest.y >= m_height )
					{
						continue;
					}

					uint32_t nearest_weight = s_get_next_point_weight( prev, nearest );
					uint32_t nearest_cell_weight = weight + nearest_weight;

					cell * nearest_cell = this->getCell( nearest );

					if( ( nearest_cell->block_mask != 0 ) || 
						( nearest_cell->block_revision == m_revision && nearest_cell->weight <= nearest_cell_weight ) )
					{
						continue;
					}

					nearest_cell->block_revision = m_revision;
					nearest_cell->weight = nearest_cell_weight;

					m_walker_false.add( nearest );
				}

				return false;
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
}