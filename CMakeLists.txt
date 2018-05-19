cmake_minimum_required(VERSION 3.0)

PROJECT(fastpathfinder)

OPTION(LIBFPF_EXTERNAL_BUILD "LIBFPF_EXTERNAL_BUILD" OFF)
OPTION(LIBFPF_EXAMPLES_BUILD "LIBFPF_EXAMPLES_BUILD" ON)

IF( NOT LIBFPF_EXTERNAL_BUILD )
    SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/${CMAKE_GENERATOR} )
    SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_GENERATOR} )    
ENDIF()

MACRO( ADD_FILTER group_name )
	SOURCE_GROUP( ${group_name} FILES ${ARGN} )
	SET( SRC_FILES ${SRC_FILES} ${ARGN} )
ENDMACRO()

ADD_FILTER(
config
	include/fastpathfinder/config.h
)

ADD_FILTER(
utils
	include/fastpathfinder/point.h
    include/fastpathfinder/helper.h
)

ADD_FILTER(
map
    include/fastpathfinder/map.h
    include/fastpathfinder/map_cell.h
    include/fastpathfinder/map_test_wall.h
)

ADD_FILTER(
pathfinder
	include/fastpathfinder/pathfinder.h
    include/fastpathfinder/pathfinder_cell.h
)

ADD_FILTER(
dummy
	src/dummy.cpp
)

INCLUDE_DIRECTORIES( ${PROJECT_NAME} include ) 

ADD_LIBRARY( ${PROJECT_NAME} STATIC ${SRC_FILES} )

if( LIBFPF_EXAMPLES_BUILD )
    set(
    Sandbox_SOURCES
        sandbox/test.cpp
    )
    
    INCLUDE_DIRECTORIES( Sandbox ../include )

    ADD_EXECUTABLE( Sandbox ${Sandbox_SOURCES} )

    TARGET_LINK_LIBRARIES( Sandbox fastpathfinder )
endif()