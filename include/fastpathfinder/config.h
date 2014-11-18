#	pragma once

#	ifndef FASTPATHFINDER_MALLOC
#	include "stdex/allocator.h"
#	define FASTPATHFINDER_MALLOC(S) stdex_malloc(S)
#	endif

#	ifndef FASTPATHFINDER_FREE
#	include "stdex/allocator.h"
#	define FASTPATHFINDER_FREE(B, S) stdex_free(B)
#	endif 

#	ifndef FASTPATHFINDER_VECTOR
#	include <stdex/stl_vector.h>
#	define FASTPATHFINDER_VECTOR stdex::vector
#	endif

#	ifndef FASTPATHFINDER_LIST
#	include <stdex/stl_list.h>
#	define FASTPATHFINDER_LIST stdex::list
#	endif
