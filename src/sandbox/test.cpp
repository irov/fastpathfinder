#	include "fastpathfinder/map.h"
#	include "fastpathfinder/pathfinder.h"
#	include "fastpathfinder/graph.h"

#	include "stdlib.h"
#	include "stdio.h"
#	include "time.h"

#	include <map>

static void print_map( fastpathfinder::map & m, size_t _bx, size_t _by, size_t _ex, size_t _ey )
{
	for( size_t j = _by; j != _ey; ++j )
	{
		for( size_t i = _bx; i != _ex; ++i )
		{
			uint32_t mask;
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
static void print_walk( fastpathfinder::map & m, fastpathfinder::pathfinder<> & pf, size_t _bx, size_t _by, size_t _ex, size_t _ey )
{
	uint32_t revision = pf.getRevision();
	for( size_t j = _by; j != _ey; ++j )
	{
		for( size_t i = _bx; i != _ex; ++i )
		{
			uint32_t mask;
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

				if( c->revision == revision )
				{
					printf("*");
				}
				else
				{
					printf(".");
				}				
			}
		}

		printf("\n");
	}
}
//////////////////////////////////////////////////////////////////////////
static bool is_path( const fastpathfinder::point_array & _path, size_t _i, size_t _j )
{
	for( size_t k = 0; k != _path.size(); ++k )
	{
		fastpathfinder::point p = _path[k];

		if( p.x == _i && p.y == _j )
		{
			return true;
		}
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
static void print_path( fastpathfinder::map & m, fastpathfinder::pathfinder<> & pf, size_t _bx, size_t _by, size_t _ex, size_t _ey )
{
	uint32_t revision = pf.getRevision();
	for( size_t j = _by; j != _ey; ++j )
	{
		for( size_t i = _bx; i != _ex; ++i )
		{
			uint32_t mask;
			if( m.getCellMask( i, j, mask ) == false )
			{
				continue;
			}

			const fastpathfinder::point_array & path = pf.getPath();
			const fastpathfinder::point_array & path_filter = pf.getPathFilter();

			if( mask > 0 )
			{
				printf("#");
			}
			else if( is_path( path_filter, i, j ) == true )
			{				
				printf("+");

			}
			else if( is_path( path, i, j ) == true )
			{				
				printf("o");
			
			}
			else
			{
				fastpathfinder::point p(i, j);
				fastpathfinder::pathfinder_cell * c = pf.getCell( p );
				
				if( c->revision == revision )
				{
					printf("*");
				}
				else
				{
					printf(".");
				}				
			}
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

	size_t count = width * height * 0.6f;
	for( size_t i = 0; i != count; ++i )
	{
		size_t x = 20 + rand() % (width - 30);
		size_t y = 20 + rand() % (height - 30);

		m.setCellMask( x, y, 1 );
	}

	uint32_t x = 2;
	uint32_t y = 2;
	uint32_t tx = width - 2;
	uint32_t ty = height - 2;

	m.setCellMask( x, y, 0 );
	m.setCellMask( tx, ty, 0 );
	
	//m.setCellMask( tx + 1, ty, 1 );
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
	while( pf.findPathNext( found ) == false )
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
	bool s = pf.findProcces();

	if( s == false )
	{
		return false;
	}

	pf.findFilter();

	system("CLS");
	print_path( m, pf, 0, 0, width, height );
	//}
		
	return found;
}

void test1()
{
	fastpathfinder::map m;

	size_t width = 70;
	size_t height = 70;

	m.initialize( width, height );

	while(true)
	{
		bool s = test( m );
		
		if( s == true )
		{
			printf("\nwin!!\n");
		}
				
		char str[80];
		gets(str);
		system("CLS");
	}
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
		bool operator() ( key _l, key _r )
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


	g.addNode( &gn0 );
	g.addNode( &gn1 );
	g.addNode( &gn2 );
	g.addNode( &gn3 );
	g.addNode( &gn4 );
	g.addNode( &gn5 );
	g.addNode( &gn6 );
	g.addNode( &gn7 );
	g.addNode( &gn8 );
	g.addNode( &gn9 );

	g.addEdge2( &gn0, &gn6, 2 );
	g.addEdge2( &gn0, &gn7, 2 );
	g.addEdge2( &gn6, &gn3, 1 );
	g.addEdge2( &gn3, &gn2, 1 );
	g.addEdge2( &gn0, &gn1, 2 );
	g.addEdge2( &gn7, &gn1, 1 );
	g.addEdge2( &gn1, &gn2, 1 );
	g.addEdge2( &gn1, &gn9, 3 );
	g.addEdge2( &gn9, &gn5, 3 );
	g.addEdge2( &gn5, &gn8, 2 );
	g.addEdge2( &gn4, &gn8, 2 );
	g.addEdge2( &gn4, &gn5, 2 );
	g.addEdge2( &gn2, &gn4, 2 );

	g.blockNode( &gn2, true );

	fastpathfinder::vector_graph_node path;
	g.getPath( &gn8, &gn7, path );

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
	//test1();
	test2();
}