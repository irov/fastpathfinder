#	pragma once

#	include <stdint.h>

namespace fastpathfinder
{
	//////////////////////////////////////////////////////////////////////////
	struct pathfinder_cell
	{
	public:
		pathfinder_cell()
			: revision(0)
			, cost(0)
		{
		}

	public:
		uint32_t revision;
		uint32_t cost;
	};
}