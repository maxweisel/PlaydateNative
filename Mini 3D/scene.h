//
//  scene.h
//  Extension
//
//  Created by Dave Hayden on 10/7/15.
//  Copyright © 2015 Panic, Inc. All rights reserved.
//

#ifndef scene_h
#define scene_h

#include <stdint.h>
#include "3dmath.h"
#include "shape.h"

typedef struct
{
	Point3D* p1; // pointers into shape's points array
	Point3D* p2;
	Point3D* p3;
	Vector3D normal;
	float colorBias; // added to lighting computation
} FaceInstance;

typedef enum
{
	kRenderInheritStyle		= 0,
	kRenderFilled			= (1<<0),
	kRenderWireframe		= (1<<1),
	kRenderWireframeBack	= (1<<2),
	kRenderWireframeWhite	= (1<<3) // 0 = black, 1 = white
} RenderStyle;

struct ShapeInstance
{
	Shape3D* prototype; // pointer to original shape

	int nPoints;
	Point3D* points;
	int nFaces;
	FaceInstance* faces;
	Vector3D offset;
	Point3D center;
	float colorBias;
	RenderStyle renderStyle;
	
	struct ShapeInstance* next;
};

typedef struct ShapeInstance ShapeInstance;
typedef struct Scene3DNode Scene3DNode;

struct Scene3DNode
{
	Matrix3D transform;
	Scene3DNode* parentNode;
	int nChildren;
	Scene3DNode* childNodes;
	ShapeInstance* shapes;
	float colorBias;
	RenderStyle renderStyle;
	int isVisible:1;
	int needsUpdate:1;
};

void Scene3DNode_init(Scene3DNode* node);
void Scene3DNode_setTransform(Scene3DNode* node, Matrix3D* xform);
void Scene3DNode_addTransform(Scene3DNode* node, Matrix3D* xform);
void Scene3DNode_addShape(Scene3DNode* node, Shape3D* shape, Vector3D offset);
Scene3DNode* Scene3DNode_newChild(Scene3DNode* node);
void Scene3DNode_setColorBias(Scene3DNode* node, float bias);
void Scene3DNode_setRenderStyle(Scene3DNode* node, RenderStyle style);
RenderStyle Scene3DNode_getRenderStyle(Scene3DNode* node);
void Scene3DNode_setVisible(Scene3DNode* node, int visible);

typedef struct
{
	int hasPerspective : 1;
	
	Matrix3D camera;
	Vector3D light;
	
	// location of the Z vanishing point on the screen. (0,0) is top left corner, (1,1) is bottom right. Defaults to (0.5,0.5)
	float centerx;
	float centery;
	
	// display scaling factor. Default is 120, so that the screen extents are (-1.66,1.66)x(-1,1)
	float scale;
	
	Scene3DNode root;
	
	ShapeInstance** shapelist;
	int shapelistsize;

} Scene3D;

void Scene3D_init(Scene3D* scene);
void Scene3D_deinit(Scene3D* scene);
void Scene3D_setCamera(Scene3D* scene, Point3D origin, Point3D lookAt, float scale, Vector3D up);
void Scene3D_setGlobalLight(Scene3D* scene, Vector3D light);
Scene3DNode* Scene3D_getRootNode(Scene3D* scene);
void Scene3D_draw(Scene3D* scene, uint8_t* buffer, int rowstride);
void Scene3D_setCenter(Scene3D* scene, float x, float y);

#endif /* scene_h */
