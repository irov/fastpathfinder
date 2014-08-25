#	pragma once

#	ifndef FASTPATHFINDER_MALLOC
#	include "stdex/allocator.h"
#	define FASTPATHFINDER_MALLOC(S) stdex_malloc(S)
#	endif

#	ifndef FASTPATHFINDER_FREE
#	include "stdex/allocator.h"
#	define FASTPATHFINDER_FREE(B, S) stdex_free(B)
#	endif 

#	ifndef FASTPATHFINDER_MEMCPY
#	include "stdex/memorycopy.h"
#	define FASTPATHFINDER_MEMCPY(Dest, Src, Size) stdex::memorycopy(Dest, Src, Size)
#	endif