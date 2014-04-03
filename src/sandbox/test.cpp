#	include "fastpathfinder/fastpathfinder.h"

#	include "stdlib.h"
#	include "stdio.h"
#	include "time.h"

bool test( fastpathfinder::map<> & m )
{
	static int iii = 0;
	++iii;
	srand(iii);

	uint32_t width = m.getWidth();
	uint32_t height = m.getHeight();

	m.clear();

	size_t count = width * height * 0.2;
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

	//bool result = m.findPath( x, y, tx, ty );
	bool result = m.findPath( tx, ty, x, y );

	return result;
}

void print_map( fastpathfinder::map<> & m )
{
	uint32_t width = m.getWidth();
	uint32_t height = m.getHeight();

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
	printf("\n");
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
				const fastpathfinder::point_array & pa = m.getPath();
				size_t count = pa.size();
				fastpathfinder::point * p = pa.buffer();


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
				const fastpathfinder::point_array & pa = m.getPathFilter();
				size_t count = pa.size();
				fastpathfinder::point * p = pa.buffer();					

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
}

void test1()
{
	fastpathfinder::map<> m;

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

		print_map( m );

		char str[80];
		gets(str);
		system("CLS");
	}
}

void test2()
{
	while(true)
	{
		fastpathfinder::map<> m;

		size_t width = 1024;
		size_t height = 1024;

		m.initialize( width, height );

		while(true)
		{
			bool result = test( m );

			if( result == true )
			{
				printf("*");
			}
		}
	}
}

void main()
{
	test1();
	//test2();
}