#	include "fastpathfinder/map.h"
#	include "fastpathfinder/pathfinder.h"

#	include "stdlib.h"
#	include "stdio.h"
#	include "time.h"

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

				const fastpathfinder::point_array & wa = pf.getWalkerTrue();

				fastpathfinder::point lp = wa.back();

				if( lp.x == i && lp.y == j )
				{
					printf("@");
				}
				else if( c->revision == revision )
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
	srand(2);

	uint32_t width = m.getWidth();
	uint32_t height = m.getHeight();

	m.clear();

	size_t count = width * height * 0.3f;
	for( size_t i = 0; i != count; ++i )
	{
		size_t x = rand() % width;
		size_t y = rand() % height;

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
		system("CLS");
		print_walk( m, pf, 0, 0, width, height );
		printf("!");
	}

	system("CLS");
	print_walk( m, pf, 0, 0, width, height );

	bool s = pf.findProcces();
		
	return found;
}

void test1()
{
	fastpathfinder::map m;

	size_t width = 60;
	size_t height = 20;

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
void main()
{
	test1();
}