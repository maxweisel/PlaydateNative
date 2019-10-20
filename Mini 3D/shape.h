//
//  shape.h
//  Extension
//
//  Created by Dave Hayden on 10/7/15.
//  Copyright © 2015 Panic, Inc. All rights reserved.
//

#ifndef shape_h
#define shape_h

#include "3dmath.h"

typedef struct
{
	int p1;
	int p2;
	int p3;
	float colorBias;
} Face3D;

typedef struct
{
	int nPoints;
	Point3D* points;
	int nFaces;
	Face3D* faces;
	Point3D center; // used for z-sorting entire shapes at a time
	float colorBias;
} Shape3D;

void Shape3D_init(Shape3D* shape);
void Shape3D_addFace(Shape3D* shape, Point3D* a, Point3D* b, Point3D* c, float colorBias);

void Shape3D_initWithPrototype(Shape3D* shape, Shape3D* proto);

#endif /* shape_h */
