//
//  shape.c
//  Extension
//
//  Created by Dave Hayden on 10/7/15.
//  Copyright © 2015 Panic, Inc. All rights reserved.
//

#include "mini3d.h"
#include "shape.h"

void Shape3D_init(Shape3D* shape)
{
	shape->nPoints = 0;
	shape->points = NULL;
	shape->nFaces = 0;
	shape->faces = NULL;
	shape->center.x = 0;
	shape->center.y = 0;
	shape->center.z = 0;
	shape->colorBias = 0;
}

int Shape3D_addPoint(Shape3D* shape, Point3D* p)
{
	int i;
	
	for ( i = 0; i < shape->nPoints; ++i )
	{
		if ( Point3D_equals(*p, shape->points[i]) )
			return i;
	}
	
	shape->points = m3d_realloc(shape->points, (shape->nPoints + 1) * sizeof(Point3D));
	
	Point3D* point = &shape->points[shape->nPoints];
	*point = *p;
	
	return shape->nPoints++;
}

void Shape3D_addFace(Shape3D* shape, Point3D* a, Point3D* b, Point3D* c, float colorBias)
{
	int p1 = Shape3D_addPoint(shape, a);
	int p2 = Shape3D_addPoint(shape, b);
	int p3 = Shape3D_addPoint(shape, c);
	
	shape->faces = m3d_realloc(shape->faces, (shape->nFaces + 1) * sizeof(Face3D));
	
	Face3D* face = &shape->faces[shape->nFaces];
	face->p1 = p1;
	face->p2 = p2;
	face->p3 = p3;
	face->colorBias = colorBias;
	
	++shape->nFaces;
	
	shape->center.x += (a->x + b->x + c->x) / 3 / shape->nFaces;
	shape->center.y += (a->y + b->y + c->y) / 3 / shape->nFaces;
	shape->center.z += (a->z + b->z + c->z) / 3 / shape->nFaces;
}
