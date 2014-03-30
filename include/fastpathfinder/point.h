#	pragma once

#	include <stdint.h>

namespace fastpathfinder
{
	//////////////////////////////////////////////////////////////////////////
	struct point
	{
		point()
		{
		}

		point( uint32_t _x, uint32_t _y )
			: x(_x)
			, y(_y)
		{
		}

		uint32_t x;
		uint32_t y;
	};
}