#	pragma once

#	include <stdint.h>

#	include "fastpathfinder/cell.h"
#	include "fastpathfinder/point.h"
#	include "fastpathfinder/point_array.h"

namespace fastpathfinder
{
	//////////////////////////////////////////////////////////////////////////
	class map
	{
	public:
		map();

	public:
		bool initialize( uint32_t _width, uint32_t _height );
		void clear();

	public:
		uint32_t getWidth() const;
		uint32_t getHeight() const;
		uint32_t getRevision() const;

	public:
		bool setCellMask( uint32_t _x, uint32_t _y, uint32_t _mask );
		uint32_t getCellMask( uint32_t _x, uint32_t _y ) const;
				
	public:
		bool findPath( uint32_t _fromX, uint32_t _fromY, uint32_t _toX, uint32_t _toY );
		point * getPath( size_t & _size );
		point * getPathFilter( size_t & _size );
			
	public:
		cell * getCell( point _point ) const;

	protected:
		bool walkerProcessFirst( point _from, point _to );
		bool walkerBresenham( point _from, point _to );
		bool findProcces( point _from, point _to );
		bool findAround( point _point );
		bool testWall( point _point, uint32_t _angle ) const;
		bool testWallBresenham( point _from, point _to ) const;
		bool nextWalk( point _prev, uint32_t _weight, uint32_t _angle, uint32_t _index, point_array & _array );

		void findFilter();

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