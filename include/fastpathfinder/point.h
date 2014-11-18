#	pragma once

#	include <stdint.h>

namespace fastpathfinder
{
	//////////////////////////////////////////////////////////////////////////
#	pragma pack(push, 1)
	struct point
	{
		point()
		{
		}

		point( uint16_t _x, uint16_t _y )
			: x(_x)
			, y(_y)
		{
		}

		uint16_t x;
		uint16_t y;
	};
#	pragma pack(pop)
}