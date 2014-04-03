#	include "fastpathfinder/fastpathfinder.h"

#	include "stdlib.h"
#	include "stdio.h"
#	include "time.h"

bool test( fastpathfinder::map & m )
{
	static int iii = 0;
	++iii;
	srand(iii);

	uint32_t width = m.getWidth();
	uint32_t height = m.getHeight();

	m.clear();

	for( size_t i = 0; i != width * height * 0.2; ++i )
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

	bool result = m.findPath( x, y, tx, ty );

	if( result == false )
	{
		return false;
	}

	for( size_t j = 0; j != height; ++j )
	{
		for( size_t i = 0; i != width; ++i )
		{
			uint32_t mask = m.getCellMask( i, j );

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

	printf("\n");
	printf("%d\n", iii);
	printf("\n");

	uint32_t revision = m.getRevision();

	for( size_t j = 0; j != height; ++j )
	{
		for( size_t i = 0; i != width; ++i )
		{
			fastpathfinder::point p(i,j);
			fastpathfinder::cell * c = m.getCell( p );

			if( c->block_mask == 0 && c->weight > 0 && c->block_revision == revision )
			{
				printf("*");
			}
			else if( c->block_mask == 0 && c->block_revision != revision )
			{
				printf(" ");
			}
			else
			{
				printf("#");
			}
		}

		printf("\n");
	}

	printf("\n");
	printf("\n");
	printf("\n");

	for( size_t j = 0; j != height; ++j )
	{
		for( size_t i = 0; i != width; ++i )
		{
			fastpathfinder::point pp(i,j);
			fastpathfinder::cell * c = m.getCell( pp );

			if( c->block_mask == 0 )
			{
				size_t count;
				fastpathfinder::point * p = m.getPath( count );

				bool exist = false;
				for( size_t k = 0; k != count; ++k )
				{
					if( p[k].x == i && p[k].y == j )
					{
						exist = true;
						break;
					}
				}

				if( exist == true )
				{
					printf(".");
				}
				else
				{
					printf(" ");
				}
			}
			else
			{
				printf("#");
			}
		}

		printf("\n");
	}

	printf("\n");
	printf("\n");
	printf("\n");

	m.findFilter();

	for( size_t j = 0; j != height; ++j )
	{
		for( size_t i = 0; i != width; ++i )
		{
			fastpathfinder::point pp(i,j);
			fastpathfinder::cell * c = m.getCell( pp );

			if( c->block_mask == 0 )
			{
				size_t count;
				fastpathfinder::point * p = m.getPathFilter( count );

				bool exist = false;
				for( size_t k = 0; k != count; ++k )
				{
					if( p[k].x == i && p[k].y == j )
					{
						exist = true;
						break;
					}
				}

				if( exist == true )
				{
					printf(".");
				}
				else
				{
					printf(" ");
				}
			}
			else
			{
				printf("#");
			}
		}

		printf("\n");
	}

	printf("\n");

	return true;
}

void main()
{
	fastpathfinder::map m;

	size_t width = 60;
	size_t height = 20;

	m.initialize( width, height );

	while(true)
	{
		if( test( m ) == false )
		{
			system("CLS");
			continue;
		}

		char str[80];
		gets(str);
		system("CLS");
	}
}