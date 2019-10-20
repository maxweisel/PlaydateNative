//
//  render.c
//  Extension
//
//  Created by Dave Hayden on 10/20/15.
//  Copyright © 2015 Panic, Inc. All rights reserved.
//

#include <stdint.h>
#include "render.h"

#define LCD_ROWS 240
#define LCD_COLUMNS 400

#if !defined(MIN)
#define MIN(a, b) (((a)<(b))?(a):(b))
#endif

#if !defined(MAX)
#define MAX(a, b) (((a)>(b))?(a):(b))
#endif

static inline void
_drawMaskPattern(uint32_t* p, uint32_t mask, uint32_t color)
{
	if ( mask == 0xffffffff )
		*p = color;
	else
		*p = (*p & ~mask) | (color & mask);
}

static inline uint32_t swap(uint32_t n)
{
#if TARGET_PLAYDATE
	//return __REV(n);
	uint32_t result;
	
	__asm volatile ("rev %0, %1" : "=l" (result) : "l" (n));
	return(result);
#else
	return ((n & 0xff000000) >> 24) | ((n & 0xff0000) >> 8) | ((n & 0xff00) << 8) | (n << 24);
#endif
}

static inline void
drawFragment(uint8_t* bitmap, int rowstride, int y, int x1, int x2, uint32_t color)
{
	if ( y < 0 || y >= LCD_ROWS || x2 < 0 || x1 >= LCD_COLUMNS )
		return;
	
	if ( x1 < 0 )
		x1 = 0;
	
	if ( x2 > LCD_COLUMNS )
		x2 = LCD_COLUMNS;
	
	if ( x1 > x2 )
		return;
	
	// Operate on 32 bits at a time
	
	int startbit = x1 % 32;
	uint32_t startmask = swap((1 << (32 - startbit)) - 1);
	int endbit = x2 % 32;
	uint32_t endmask = swap(((1 << endbit) - 1) << (32 - endbit));
	
	int col = x1 / 32;
	uint32_t* p = (uint32_t*)&bitmap[y * rowstride + 4*col];
	
	if ( col == x2 / 32 )
	{
		uint32_t mask = 0;
		
		if ( startbit > 0 && endbit > 0 )
			mask = startmask & endmask;
		else if ( startbit > 0 )
			mask = startmask;
		else if ( endbit > 0 )
			mask = endmask;
		
		_drawMaskPattern(p, mask, color);
	}
	else
	{
		int x = x1;
		
		if ( startbit > 0 )
		{
			_drawMaskPattern(p++, startmask, color);
			x += (32 - startbit);
		}
		
		while ( x + 32 <= x2 )
		{
			_drawMaskPattern(p++, 0xffffffff, color);
			x += 32;
		}

		/* Interesting! This "optimization" of the above loop isn't any faster.
		
		switch ( x2/32 - x/32 )
		{
			// max 400/32 = 12
			case 12: *p++ = color;
			case 11: *p++ = color;
			case 10: *p++ = color;
			case 9: *p++ = color;
			case 8: *p++ = color;
			case 7: *p++ = color;
			case 6: *p++ = color;
			case 5: *p++ = color;
			case 4: *p++ = color;
			case 3: *p++ = color;
			case 2: *p++ = color;
			case 1: *p++ = color;
		}
		*/
		
		if ( endbit > 0 )
			_drawMaskPattern(p, endmask, color);
	}
}

struct dda
{
	int x;
	int endx;
	int dx;
	int dy;
	int d; // while non-negative, move left/right, subtract dy; move down, add dx
};

static inline int dda_nextX(struct dda* dda)
{
	if ( dda->dx > 0 )
	{
		while ( dda->d >= 0 && dda->x < dda->endx )
		{
			++dda->x;
			dda->d -= dda->dy;
		}
		
		dda->d += dda->dx;
	}
	else
	{
		while ( dda->d >= 0 && dda->x > dda->endx )
		{
			--dda->x;
			dda->d -= dda->dy;
		}
		
		dda->d -= dda->dx;
	}
	
	return dda->x;
}

#define abs(a) (((a)>=(0))?(a):-(a))

static void dda_advance(struct dda* dda, int n)
{
	int d = dda->d + n * abs(dda->dx); // d after moving down n rows
	int c = 0;
	
	if ( dda->dy != 0 )
		c = (d + dda->dy - 1) / dda->dy; // how many times do we have to subtract dy to make d negative?
	
	if ( dda->dx > 0 )
		dda->x += c;
	else
		dda->x -= c;
	
	dda->d = d - c * dda->dy;
}

static void dda_start(struct dda* dda, int x1, int y1, int x2, int y2)
{
	dda->x = x1;
	dda->endx = x2;
	dda->dx = x2 - x1;
	dda->dy = y2 - y1;
	
	if ( dda->dy > abs(dda->dx) )
		dda->d = -dda->dy / 2;
	else
		dda->d = abs(dda->dx) / 2;
}

LCDRowRange
drawLine(uint8_t* bitmap, int rowstride, int x1, int y1, int x2, int y2, int thick, uint32_t color)
{
	if ( y2 < y1 )
	{
		int tmp = x1;
		x1 = x2;
		x2 = tmp;
		
		tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	
	// adjust end point for butt end cap style
//	if ( endCapStyle == kLineCapStyleButt ) {
//		if ( abs(x1-x2) > abs(y1-y2) ) if (x2 < x1) x1 = x1 - 1; else x2 = x2 - 1; else y2 = y2 - 1;
//	}
	
	int lastx = x1;
	int y = y1;
	int endy = MIN(LCD_ROWS, y2);
	
	if ( y >= LCD_ROWS || endy < 0 || MIN(x1, x2) >= LCD_COLUMNS || MAX(x1, x2) < 0 )
		return (LCDRowRange){ 0, 0 };
	
	struct dda dda;
	dda_start(&dda, x1, y1, x2, y2);
	
	if ( y < 0 )
	{
		dda_advance(&dda, -1 - y);
		y = -1;
		lastx = dda.x;
	}
	
	while ( y <= endy )
	{
		int x = dda_nextX(&dda);
		
		if ( thick == 0 && x == lastx )
			drawFragment(bitmap, rowstride, y, lastx, lastx + 1, color);
		else if ( dda.dx < 0 )
			drawFragment(bitmap, rowstride, y, x, lastx + thick, color);
		else
			drawFragment(bitmap, rowstride, y, lastx, x + thick, color);
		
		lastx = dda.x;
		++y;
	}
	
	return (LCDRowRange){ MAX(0, y1), MIN(LCD_ROWS, y2) };
}


typedef uint8_t Pattern[8];

static Pattern patterns[] =
{
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x80, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00 },
	{ 0x88, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00 },
	{ 0x88, 0x00, 0x20, 0x00, 0x88, 0x00, 0x02, 0x00 },
	{ 0x88, 0x00, 0x22, 0x00, 0x88, 0x00, 0x22, 0x00 },
	{ 0xa8, 0x00, 0x22, 0x00, 0x8a, 0x00, 0x22, 0x00 },
	{ 0xaa, 0x00, 0x22, 0x00, 0xaa, 0x00, 0x22, 0x00 },
	{ 0xaa, 0x00, 0xa2, 0x00, 0xaa, 0x00, 0x2a, 0x00 },
	{ 0xaa, 0x00, 0xaa, 0x00, 0xaa, 0x00, 0xaa, 0x00 },
	{ 0xaa, 0x40, 0xaa, 0x00, 0xaa, 0x04, 0xaa, 0x00 },
	{ 0xaa, 0x44, 0xaa, 0x00, 0xaa, 0x44, 0xaa, 0x00 },
	{ 0xaa, 0x44, 0xaa, 0x10, 0xaa, 0x44, 0xaa, 0x01 },
	{ 0xaa, 0x44, 0xaa, 0x11, 0xaa, 0x44, 0xaa, 0x11 },
	{ 0xaa, 0x54, 0xaa, 0x11, 0xaa, 0x45, 0xaa, 0x11 },
	{ 0xaa, 0x55, 0xaa, 0x11, 0xaa, 0x55, 0xaa, 0x11 },
	{ 0xaa, 0x55, 0xaa, 0x51, 0xaa, 0x55, 0xaa, 0x15 },
	{ 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55 },
	{ 0xba, 0x55, 0xaa, 0x55, 0xab, 0x55, 0xaa, 0x55 },
	{ 0xbb, 0x55, 0xaa, 0x55, 0xbb, 0x55, 0xaa, 0x55 },
	{ 0xbb, 0x55, 0xea, 0x55, 0xbb, 0x55, 0xae, 0x55 },
	{ 0xbb, 0x55, 0xee, 0x55, 0xbb, 0x55, 0xee, 0x55 },
	{ 0xfb, 0x55, 0xee, 0x55, 0xbf, 0x55, 0xee, 0x55 },
	{ 0xff, 0x55, 0xee, 0x55, 0xff, 0x55, 0xee, 0x55 },
	{ 0xff, 0x55, 0xfe, 0x55, 0xff, 0x55, 0xef, 0x55 },
	{ 0xff, 0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0x55 },
	{ 0xff, 0x55, 0xff, 0xd5, 0xff, 0x55, 0xff, 0x5d },
	{ 0xff, 0x55, 0xff, 0xdd, 0xff, 0x55, 0xff, 0xdd },
	{ 0xff, 0x75, 0xff, 0xdd, 0xff, 0x57, 0xff, 0xdd },
	{ 0xff, 0x77, 0xff, 0xdd, 0xff, 0x77, 0xff, 0xdd },
	{ 0xff, 0x77, 0xff, 0xfd, 0xff, 0x77, 0xff, 0xdf },
	{ 0xff, 0x77, 0xff, 0xff, 0xff, 0x77, 0xff, 0xff },
	{ 0xff, 0xf7, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff },
	{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }
};


static LCDRowRange
LCDBitmap_fillMonotonePolygon(uint8_t* bitmap, int rowstride, int* leftx, int* lefty, int nleft, int* rightx, int* righty, int nright, Pattern pattern)
{
	// run differential analyzer on the edges, draw a fragment left to right for each row
	
	struct dda leftdda;
	struct dda rightdda;
	
	dda_start(&leftdda, leftx[0], lefty[0], leftx[1], lefty[1]);
	dda_start(&rightdda, rightx[0], righty[0], rightx[1], righty[1]);
	
	int y = lefty[0];
	int endy = MIN(LCD_ROWS, lefty[nleft-1]);
	
	if ( y > LCD_ROWS || endy < 0 )
		return (LCDRowRange){ 0, 0 };
	
	int ln = 0;
	int rn = 0;
	
	while ( y < 0 )
	{
		int nextl = (ln < nleft - 1) ? lefty[ln+1] : INT32_MAX;
		int nextr = (rn < nright - 1) ? righty[rn+1] : INT32_MAX;
		
		if ( nextl < 0 && nextl < nextr )
		{
			++ln;
			
			dda_advance(&leftdda, nextl - y);
			dda_advance(&rightdda, nextl - y);
			
			dda_start(&leftdda, leftx[ln], lefty[ln], leftx[ln+1], lefty[ln+1]);
			
			y = nextl;
		}
		else if ( nextr < 0 )
		{
			++rn;
			
			dda_advance(&leftdda, nextr - y);
			dda_advance(&rightdda, nextr - y);
			
			dda_start(&rightdda, rightx[rn], righty[rn], rightx[rn+1], righty[rn+1]);
			
			y = nextr;
		}
		else
		{
			dda_advance(&leftdda, -y);
			dda_advance(&rightdda, -y);
			y = 0;
		}
	}
	
	while ( y <= endy && ln < nleft && rn < nright )
	{
		if ( lefty[ln+1] == y )
		{
			if ( ++ln == nleft - 1 )
				break;
			
			dda_start(&leftdda, leftx[ln], lefty[ln], leftx[ln+1], lefty[ln+1]);
		}
		
		if ( righty[rn+1] == y )
		{
			if ( ++rn == nright - 1 )
				break;
			
			dda_start(&rightdda, rightx[rn], righty[rn], rightx[rn+1], righty[rn+1]);
		}
		
		uint8_t p = pattern[y%8];
		uint32_t color = (p<<24) | (p<<16) | (p<<8) | p;
		
		drawFragment(bitmap, rowstride, y, dda_nextX(&leftdda), dda_nextX(&rightdda), color);
		
		++y;
	}
	
	return (LCDRowRange){ MAX(0, lefty[0]), MIN(LCD_ROWS, y - 1) };
}

LCDRowRange
fillTriangle(uint8_t* bitmap, int rowstride, float x1, float y1, float x2, float y2, float x3, float y3, float v)
{
	int vi = (int)(32 * v);
	
	if ( vi > 32 )
		vi = 32;
	else if ( vi < 0 )
		vi = 0;
	
	uint8_t* pattern = (uint8_t*)&patterns[vi];

	int ax, ay, bx, by, cx, cy;
	
	if ( y1 <= y2 && y1 < y3 )
	{
		ax = x1; ay = y1;
		
		if ( y2 < y3 )
		{
			bx = x2; by = y2;
			cx = x3; cy = y3;
		}
		else
		{
			bx = x3; by = y3;
			cx = x2; cy = y2;
		}
	}
	else if ( y2 < y1 && y2 < y3 )
	{
		ax = x2; ay = y2;
		
		if ( y1 < y3 )
		{
			bx = x1; by = y1;
			cx = x3; cy = y3;
		}
		else
		{
			bx = x3; by = y3;
			cx = x1; cy = y1;
		}
	}
	else
	{
		ax = x3; ay = y3;
		
		if ( y1 < y2 )
		{
			bx = x1; by = y1;
			cx = x2; cy = y2;
		}
		else
		{
			bx = x2; by = y2;
			cx = x1; cy = y1;
		}
	}
	
	int leftx[3];
	int lefty[3];
	int rightx[3];
	int righty[3];
	
	leftx[0] = rightx[0] = ax;
	lefty[0] = righty[0] = ay;
	
	if ( (bx - ax) * (cy - ay) > (cx - ax) * (by - ay) )
	{
		// b is to the right of ac
		
		rightx[1] = bx;
		righty[1] = by;
		
		leftx[1] = rightx[2] = cx;
		lefty[1] = righty[2] = cy;
		
		return LCDBitmap_fillMonotonePolygon(bitmap, rowstride, leftx, lefty, 2, rightx, righty, 3, pattern);
	}
	else
	{
		// b is to the left
		
		leftx[1] = bx;
		lefty[1] = by;
		
		leftx[2] = rightx[1] = cx;
		lefty[2] = righty[1] = cy;
		
		return LCDBitmap_fillMonotonePolygon(bitmap, rowstride, leftx, lefty, 3, rightx, righty, 2, pattern);
	}
}

/*
LCDRowRange
fillTriangle(uint8_t* bitmap, int rowstride, float x1, float y1, float x2, float y2, float x3, float y3, float v)
{
	int vi = (int)(32 * v);

	if ( vi > 32 )
		vi = 32;
	else if ( vi < 0 )
		vi = 0;
	
	uint8_t* pattern = (uint8_t*)&patterns[vi];
	
	// Bounding rectangle
	int minx = MAX(0, MIN(x1, MIN(x2, x3)));
	int maxx = MIN(LCD_COLUMNS, MAX(x1, MAX(x2, x3)));
	int miny = MAX(0, MIN(y1, MIN(y2, y3)));
	int maxy = MIN(LCD_ROWS, MAX(y1, MAX(y2, y3)));
	
	int c1 = (x1 - x2) * (miny - y1) - (y1 - y2) * (minx - x1);
	int c2 = (x2 - x3) * (miny - y2) - (y2 - y3) * (minx - x2);
	int c3 = (x3 - x1) * (miny - y3) - (y3 - y1) * (minx - x3);

	int d1dx = y2 - y1;
	int d2dx = y3 - y2;
	int d3dx = y1 - y3;
	
	int d1dy = x1 - x2;
	int d2dy = x2 - x3;
	int d3dy = x3 - x1;
	
	int y;

	for ( y = miny; y < maxy; ++y )
	{
		int x = minx;
		int d1 = c1;
		int d2 = c2;
		int d3 = c3;
		
		int startx = 0;
		int endx = 0;
		
		for ( x = minx; x < maxx; ++x )
		{
			if ( (d1 >= 0 && d2 >= 0 && d3 >= 0) || (d1 <= 0 && d2 <= 0 && d3 <= 0) )
				break;
			
			d1 += d1dx;
			d2 += d2dx;
			d3 += d3dx;
		}

		if ( x < maxx )
		{
			startx = x;

			for ( ; x < maxx; ++x )
			{
				if ( !(d1 >= 0 && d2 >= 0 && d3 >= 0) && !(d1 <= 0 && d2 <= 0 && d3 <= 0) )
					break;
				
				d1 += d1dx;
				d2 += d2dx;
				d3 += d3dx;
			}
			
			endx = x - 1;

			uint8_t p = pattern[y%8];
			uint32_t color = (p<<24) | (p<<16) | (p<<8) | p;
			
			drawFragment(bitmap, rowstride, y, startx, endx, color);
		}
		
		c1 += d1dy;
		c2 += d2dy;
		c3 += d3dy;
	}
	
	return (LCDRowRange){ miny, maxy };
}
*/
