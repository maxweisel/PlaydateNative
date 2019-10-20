//
//  render.h
//  Extension
//
//  Created by Dave Hayden on 10/20/15.
//  Copyright © 2015 Panic, Inc. All rights reserved.
//

#ifndef render_h
#define render_h

#include <stdio.h>

typedef struct
{
	int16_t start;
	int16_t end;
} LCDRowRange;

LCDRowRange fillTriangle(uint8_t* bitmap, int rowstride, float x1, float y1, float x2, float y2, float x3, float y3, float v);
LCDRowRange drawLine(uint8_t* bitmap, int rowstride, int x1, int y1, int x2, int y2, int thick, uint32_t color);

#endif /* render_h */
