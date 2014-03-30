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

	public:
		bool setCellMask( uint32_t _x, uint32_t _y, uint32_t _mask );
		uint32_t getCellMask( uint32_t _x, uint32_t _y ) const;
				
	public:
		bool findPath( uint32_t _fromX, uint32_t _fromY, uint32_t _toX, uint32_t _toY );
			
	public:
		cell * getCell( point _point ) const;

	protected:
		bool walkerProcessFirst( point _from, point _to );
		bool walkerBresenham( point _from, point _to );
		bool findProcces( point _from, point _to );
		bool findAround( point _point );

	public:
		uint32_t m_width;
		uint32_t m_height;

		cell * m_cells;
		
		uint32_t m_revision;
		uint32_t m_walkerWeight;

		point_array m_walker_true;
		point_array m_walker_false;
		point_array m_path;
	};
}