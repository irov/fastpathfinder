#	pragma once

#	include <stdint.h>

namespace fastpathfinder
{
	//////////////////////////////////////////////////////////////////////////
	static const int32_t cell_next_point_deltha[7] = {-1, 1, -2, 2, -3, 3, 4};
	//////////////////////////////////////////////////////////////////////////
	static const int16_t cell_matrix_to_deltha_x[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
	static const int16_t cell_matrix_to_deltha_y[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
	//////////////////////////////////////////////////////////////////////////
	static const int16_t cell_angle_to_deltha_x[8] = {1, 1, 0, -1, -1, -1, 0, 1};
	static const int16_t cell_angle_to_deltha_y[8] = {0, -1, -1, -1, 0, 1, 1, 1};
	//////////////////////////////////////////////////////////////////////////
	static const uint32_t cell_diagonal_cost = 140;
	static const uint32_t cell_line_cost = 99;
	static const uint32_t cell_angle_to_cost[8] = {cell_line_cost, cell_diagonal_cost, cell_line_cost, cell_diagonal_cost, cell_line_cost, cell_diagonal_cost, cell_line_cost, cell_diagonal_cost};
	static const uint32_t cell_diagonal_to_cost[3] = {0, cell_line_cost, cell_diagonal_cost};
	//////////////////////////////////////////////////////////////////////////
	static const uint32_t cell_matrix_to_angle[9] = {3, 2, 1, 4, 8, 0, 5, 6, 7};
	static const uint32_t cell_angle_to_matrix[9] = {5, 2, 1, 0, 3, 6, 7, 8, 4};
	static const bool cell_angle_to_test_wall[9] = {false, true, false, true, false, true, false, true, false};
	static const uint32_t cell_best_angle_line = 7;
	static const uint32_t cell_best_angle_diagonal = 7;
	static const uint32_t cell_best_angle_count[8] = {cell_best_angle_diagonal, cell_best_angle_line, cell_best_angle_diagonal, cell_best_angle_line, cell_best_angle_diagonal, cell_best_angle_line, cell_best_angle_diagonal, cell_best_angle_line};
	//////////////////////////////////////////////////////////////////////////
	struct bresenham_line
	{
		int16_t deltaX;
		int16_t deltaY;

		int16_t signX;
		int16_t signY;

		int16_t error;
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
	inline static uint32_t s_get_neighbour_point_angle( point _from, point _to )
	{
		uint16_t deltaX = _to.x >= _from.x ? (_to.x - _from.x) + 1 : 0;
		uint16_t deltaY = _to.y >= _from.y ? (_to.y - _from.y) + 1 : 0;

		uint16_t matrix = deltaX + deltaY * 3;

		uint32_t angle = cell_matrix_to_angle[matrix];

		return angle;
	}
	//////////////////////////////////////////////////////////////////////////
	inline static uint32_t s_get_point_angle( point _from, point _to )
	{
		uint16_t deltaX = _to.x < _from.x ? 0 : ((_to.x == _from.x) ? 1 : 2);
		uint16_t deltaY = _to.y < _from.y ? 0 : ((_to.y == _from.y) ? 1 : 2);

		uint16_t matrix = deltaX + deltaY * 3;

		uint32_t angle = cell_matrix_to_angle[matrix];

		return angle;
	}
	//////////////////////////////////////////////////////////////////////////
	inline static uint32_t s_get_next_point_cost( point _from, point _to )
	{
		uint16_t deltaX = _to.x > _from.x ? _to.x - _from.x : _from.x - _to.x;
		uint16_t deltaY = _to.y > _from.y ? _to.y - _from.y : _from.y - _to.y;

		uint16_t diagonal = deltaX + deltaY;

		uint32_t weight = cell_diagonal_to_cost[diagonal];

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

		int16_t dx = cell_matrix_to_deltha_x[matrix];
		int16_t dy = cell_matrix_to_deltha_y[matrix];

		uint16_t nx = _point.x + dx;
		uint16_t ny = _point.y + dy;

		point np(nx, ny);

		return np;
	}
	//////////////////////////////////////////////////////////////////////////
	inline static uint16_t s_point_distance( point p0, point p1 )
	{
		uint16_t dx = ( p1.x > p0.x ) ? p1.x - p0.x : p0.x - p1.x;
		uint16_t dy = ( p1.y > p0.y ) ? p1.y - p0.y : p0.y - p1.y;

		uint16_t distance = ( dy > dx ) ? ( (961 * dy + 398 * dx ) / 1000 ) : ( (961 * dx + 398 * dy ) / 1000 );

		return distance;
	}
}