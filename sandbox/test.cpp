#	include "malloc.h"

#	define FASTPATHFINDER_MALLOC(S) malloc(S)
#	define FASTPATHFINDER_FREE(B, S) free(B)

#	include <vector>
#	define FASTPATHFINDER_VECTOR std::vector

#	include <list>
#	define FASTPATHFINDER_LIST std::vector

#	include "fastpathfinder/map.h"
#	include "fastpathfinder/pathfinder.h"
#	include "fastpathfinder/graph.h"

#	include "stdlib.h"
#	include "stdio.h"
#	include "time.h"

#	include <map>

static void print_map( fastpathfinder::map & m, uint32_t _bx, uint32_t _by, uint32_t _ex, uint32_t _ey )
{
	for( uint32_t j = _by; j != _ey; ++j )
	{
		for( uint32_t i = _bx; i != _ex; ++i )
		{
			uint8_t mask;
			if( m.getCellMask( i, j, mask ) == false )
			{
				continue;
			}

			if( mask > 0 )
			{
				printf("#");
			}
			else
			{
				printf(".");
			}
		}

		printf("\n");
	}
}
//////////////////////////////////////////////////////////////////////////
static void print_walk( fastpathfinder::map & m, fastpathfinder::pathfinder<> & pf, uint32_t _bx, uint32_t _by, uint32_t _ex, uint32_t _ey )
{
	for( uint32_t j = _by; j != _ey; ++j )
	{
		for( uint32_t i = _bx; i != _ex; ++i )
		{
			uint8_t mask;
			if( m.getCellMask( i, j, mask ) == false )
			{
				continue;
			}

			if( mask > 0 )
			{
				printf("#");
			}
			else
			{
				fastpathfinder::point p(i, j);
				fastpathfinder::pathfinder_cell * c = pf.getCell( p );

				printf("*");			
			}
		}

		printf("\n");
	}
}
//////////////////////////////////////////////////////////////////////////
static bool is_path( const fastpathfinder::point_array & _path, uint32_t _i, uint32_t _j )
{
	for( fastpathfinder::point p : _path )
	{
		if( p.x == _i && p.y == _j )
		{
			return true;
		}
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
static void print_path( fastpathfinder::map & m, fastpathfinder::pathfinder<> & pf, uint32_t _bx, uint32_t _by, uint32_t _ex, uint32_t _ey )
{
	for( uint32_t j = _by; j != _ey; ++j )
	{
		for( uint32_t i = _bx; i != _ex; ++i )
		{
			uint8_t mask;
			if( m.getCellMask( i, j, mask ) == false )
			{
				continue;
			}

			//const fastpathfinder::point_array & path = pf.getPath();
			const fastpathfinder::point_array & path_filter = pf.getPathFilter();

			if( mask > 0 )
			{
				printf("#");
			}
			else if( is_path( path_filter, i, j ) == true )
			{				
				printf("o");
			
			}
			else
			{
				fastpathfinder::point p(i, j);
				fastpathfinder::pathfinder_cell * c = pf.getCell( p );
				
				printf("*");		
			}
		}

		printf("\n");
	}
}

static void print_map_cost( fastpathfinder::map & m, fastpathfinder::pathfinder<> & pf, uint32_t _bx, uint32_t _by, uint32_t _ex, uint32_t _ey )
{
	for( uint32_t j = _by; j != _ey; ++j )
	{
		for( uint32_t i = _bx; i != _ex; ++i )
		{
			fastpathfinder::point p(i, j);
			fastpathfinder::pathfinder_cell * c = pf.getCell( p );

			printf("%4d "
				, c->cost 
				);
		}

		printf("\n");
	}
}

bool test( fastpathfinder::map & m )
{
	//srand(1);

	uint32_t width = m.getWidth();
	uint32_t height = m.getHeight();

	m.clear();

	uint32_t x = 1;
	uint32_t y = 1;
	uint32_t tx = 24;
	uint32_t ty = 4;

	//m.setCellMask( x, y, 0 );
	//m.setCellMask( tx, ty, 0 );
	
	m.setCellMask( 10, 0, 9 );
	m.setCellMask( 10, 1, 9 );
	m.setCellMask( 10, 2, 9 );
	m.setCellMask( 10, 3, 9 );
	m.setCellMask( 10, 4, 9 );
	m.setCellMask( 10, 5, 9 );
	m.setCellMask( 10, 6, 9 );
	m.setCellMask( 10, 7, 9 );
	m.setCellMask( 10, 8, 9 );
	//m.setCellMask( tx, ty + 1, 1 );
	//m.setCellMask( tx - 1, ty, 1 );
	//m.setCellMask( tx, ty - 1, 1 );

	print_map( m, 0, 0, width, height );

	//bool result = m.findPath( x, y, tx, ty );
	fastpathfinder::pathfinder<> pf;
	pf.initialize( &m );

	if( pf.findPathFirst( x, y, tx, ty ) == false )
	{
		return false;
	}

	bool found;
	while( pf.walkerWave( found ) == false )
	{
		//system("CLS");
		//print_walk( m, pf, 0, 0, width, height );
		//printf("!");
	}

	if( found == false )
	{
		return false;
	}

	//system("CLS");
	//print_walk( m, pf, 0, 0, width, height );

	//if( found == true )
		//pf.findFilter();

	print_map_cost( m, pf, 0, 0, width, height );		

	bool s = pf.findPath();
	if( s == false )
	{
		return false;
	}

	pf.filterPath();

	print_path( m, pf, 0, 0, width, height );
			
	return found;
}

void test1()
{
	fastpathfinder::map m;

    uint16_t width = 25;
    uint16_t height = 10;

	m.initialize( width, height );

	bool s = test( m );
				
	//char str[80];
	//gets(str);
	//system("CLS");
}

struct node
	: public fastpathfinder::graph_node
{
	node( uint32_t _index )
		: index(_index)
	{
	}

	uint32_t index;
};

void test2()
{
	struct key
	{
		int i;
		int j;
		int k;
	};

	struct k_less
	{
		bool operator() ( const key & _l, const key & _r ) const
		{
			if( _l.i < _r.i )
			{
				return true;
			}

			if( _l.j < _r.j )
			{
				return true;
			}

			if( _l.k < _r.k )
			{
				return true;
			}

			return false;
		}
	};

	typedef std::map<key, int, k_less> M; 
	M m;

	key k0;
	k0.i = 1;
	k0.j = 2;
	k0.k = 3;

	m[k0] = 1;

	key k1;
	k1.i = -1;
	k1.j = -2;
	k1.k = -3;

	m[k1] = 2;

	m[k0] = 3;

	m[k1] = 4;

	key k2;
	k2.i = 1;
	k2.j = 2;
	k2.k = 2;

	m[k2] = 5;

	if( true & false )
	{
		printf("11");
	}

	fastpathfinder::graph g;

	node gn0(0);
	node gn1(1);
	node gn2(2);
	node gn3(3);
	node gn4(4);
	node gn5(5);
	node gn6(6);
	node gn7(7);
	node gn8(8);
	node gn9(9);


	g.addNode( &gn0, 0 );
	g.addNode( &gn1, 0 );
	g.addNode( &gn2, 0 );
	g.addNode( &gn3, 0 );
	g.addNode( &gn4, 0 );
	g.addNode( &gn5, 0 );
	g.addNode( &gn6, 0 );
	g.addNode( &gn7, 0 );
	g.addNode( &gn8, 0 );
	g.addNode( &gn9, 0 );

    g.addEdge2( &gn0, &gn6, 0, 2 );
    g.addEdge2( &gn0, &gn7, 0, 2 );
    g.addEdge2( &gn6, &gn3, 0, 1 );
    g.addEdge2( &gn3, &gn2, 0, 1 );
    g.addEdge2( &gn0, &gn1, 0, 2 );
    g.addEdge2( &gn7, &gn1, 0, 1 );
    g.addEdge2( &gn1, &gn2, 0, 1 );
    g.addEdge2( &gn1, &gn9, 0, 3 );
    g.addEdge2( &gn9, &gn5, 0, 3 );
    g.addEdge2( &gn5, &gn8, 0, 2 );
    g.addEdge2( &gn4, &gn8, 0, 2 );
    g.addEdge2( &gn4, &gn5, 0, 2 );
    g.addEdge2( &gn2, &gn4, 0, 2 );

	g.setBlock( &gn2, 1 );

	fastpathfinder::vector_graph_node path;
	g.getPath( &gn8, &gn7, 0, path );

	for( fastpathfinder::vector_graph_node::iterator
		it = path.begin(),
		it_end= path.end();
	it != it_end;
	++it )
	{
		node * n = (node *)*it;

		printf("%d\n"
			, n->index 
			);
	}
}

void main()
{
	test1();
	//test2();
}