#	pragma once

#	include <stdint.h>

namespace fastpathfinder
{
	//////////////////////////////////////////////////////////////////////////
#	pragma pack(push, 1)
	struct map_cell
	{
		uint8_t block_mask;
		uint8_t block_weight;
	};
#	pragma pack(pop)
}