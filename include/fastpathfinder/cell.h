#	pragma once

#	include <stdint.h>

namespace fastpathfinder
{
	//////////////////////////////////////////////////////////////////////////
	struct cell
	{
	public:
		cell()
			: block_revision(0)
			, block_mask(0)
			, weight(0)
		{
		}

	public:
		uint32_t block_revision;
		uint32_t block_mask;
		uint32_t weight;
	};
}