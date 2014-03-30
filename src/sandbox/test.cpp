#	include "fastpathfinder/fastpathfinder.h"

#	include "stdlib.h"
#	include "stdio.h"

void main()
{
	fastpathfinder::map m;

	m.initialize( 9, 9 );

	m.setCellMask( 1, 3, 1 );
	m.setCellMask( 2, 3, 1 );
	m.setCellMask( 3, 3, 1 );
	m.setCellMask( 4, 3, 1 );
	m.setCellMask( 5, 3, 1 );
	m.setCellMask( 4, 4, 1 );
	m.setCellMask( 4, 5, 1 );
	m.setCellMask( 4, 6, 1 );
	m.setCellMask( 4, 7, 1 );
	m.setCellMask( 4, 8, 1 );

	//m.setCellMask( 5, 6, 1 );
	//m.setCellMask( 6, 6, 1 );
	//m.setCellMask( 7, 6, 1 );
	//m.setCellMask( 8, 6, 1 );

	m.setCellMask( 1, 4, 1 );
	m.setCellMask( 1, 5, 1 );
	m.setCellMask( 1, 6, 1 );
	m.setCellMask( 1, 7, 1 );


	for( size_t j = 0; j != 9; ++j )
	{
		for( size_t i = 0; i != 9; ++i )
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

	uint32_t x = 2;
	uint32_t y = 6;
	uint32_t tx = 8;
	uint32_t ty = 8;

	bool result = m.findPath( x, y, tx, ty );

	printf("@@@ %d\n"
		, result
		);

	for( size_t j = 0; j != 9; ++j )
	{
		for( size_t i = 0; i != 9; ++i )
		{
			fastpathfinder::point p(i,j);
			fastpathfinder::cell * c = m.getCell( p );

			if( c->block_mask == 0 )
			{
				printf("%3d "
					, c->weight / 99
					);
			}
			else
			{
				printf("### ");
			}
		}

		printf("\n");
	}

	printf("\n");
}