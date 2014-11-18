#	pragma once

#	include <stdint.h>

namespace fastpathfinder
{
	//////////////////////////////////////////////////////////////////////////
#	pragma pack(push, 1)
	struct pathfinder_cell
	{
		uint32_t cost;
	};
#	pragma pack(pop)
}