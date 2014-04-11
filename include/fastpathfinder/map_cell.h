#	pragma once

#	include <stdint.h>

namespace fastpathfinder
{
	//////////////////////////////////////////////////////////////////////////
	struct map_cell
	{
	public:
		map_cell()
			: block_mask(0)
			, block_weight(1)
		{
		}

	public:
		uint32_t block_mask;
		uint8_t block_weight;
	};
}