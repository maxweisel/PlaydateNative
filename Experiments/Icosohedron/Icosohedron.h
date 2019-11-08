//
//  Icosohedron.h
//  Playdate Native
//
//  Created by Max Weisel on 10/20/19.
//  Copyright © 2019 Max Weisel. All rights reserved.
//

#pragma once

#include "Playdate.h"

using namespace Playdate;

extern "C" {
#include "mini3d.h"
#include "scene.h"
#include "shape.h"
#include <math.h>
#include "3dmath.h"
}

class Icosohedron : public Application {
public:
    Icosohedron() {
        // Configure 3D renderer
        mini3d_setRealloc(System::realloc);
        
        // Create points
        float p = (sqrtf(5.0f) - 1.0f) / 2.0f;
        
        Point3D x1 = Point3DMake( 0, -p,  1);
        Point3D x2 = Point3DMake( 0,  p,  1);
        Point3D x3 = Point3DMake( 0,  p, -1);
        Point3D x4 = Point3DMake( 0, -p, -1);
        
        Point3D y1 = Point3DMake( 1,  0,  p);
        Point3D y2 = Point3DMake( 1,  0, -p);
        Point3D y3 = Point3DMake(-1,  0, -p);
        Point3D y4 = Point3DMake(-1,  0,  p);
        
        Point3D z1 = Point3DMake(-p,  1,  0);
        Point3D z2 = Point3DMake( p,  1,  0);
        Point3D z3 = Point3DMake( p, -1,  0);
        Point3D z4 = Point3DMake(-p, -1,  0);
        
        // Create shape
        Shape3D_init(&_shape);
        Shape3D_addFace(&_shape, &z1, &y3, &y4, 0.0f);
        Shape3D_addFace(&_shape, &z1, &x3, &y3, 0.0f);
        Shape3D_addFace(&_shape, &z1, &z2, &x3, 0.0f);
        Shape3D_addFace(&_shape, &z1, &x2, &z2, 0.0f);
        Shape3D_addFace(&_shape, &z1, &y4, &x2, 0.0f);
        Shape3D_addFace(&_shape, &y4, &y3, &z4, 0.0f);
        
        Shape3D_addFace(&_shape, &z4, &y3, &x4, 0.0f);
        Shape3D_addFace(&_shape, &y3, &x3, &x4, 0.0f);
        Shape3D_addFace(&_shape, &x4, &x3, &y2, 0.0f);
        Shape3D_addFace(&_shape, &x3, &z2, &y2, 0.0f);
        Shape3D_addFace(&_shape, &y2, &z2, &y1, 0.0f);
        Shape3D_addFace(&_shape, &z2, &x2, &y1, 0.0f);
        Shape3D_addFace(&_shape, &y1, &x2, &x1, 0.0f);
        Shape3D_addFace(&_shape, &x2, &y4, &x1, 0.0f);
        Shape3D_addFace(&_shape, &x1, &y4, &z4, 0.0f);
        
        Shape3D_addFace(&_shape, &z3, &y2, &y1, 0.0f);
        Shape3D_addFace(&_shape, &z3, &y1, &x1, 0.0f);
        Shape3D_addFace(&_shape, &z3, &x1, &z4, 0.0f);
        Shape3D_addFace(&_shape, &z3, &z4, &x4, 0.0f);
        Shape3D_addFace(&_shape, &z3, &x4, &y2, 0.0f);
        
        // Create scene
        Scene3D_init(&_scene);
        Scene3D_setCamera(&_scene, (Point3D){ 0, 0, -3.0f }, (Point3D){ 0, 0, 1 }, 1.0, (Vector3D){ 0, 1, 0 });
        Scene3D_setGlobalLight(&_scene, Vector3D_normalize(Vector3DMake(0.2f, -1.0f, 0.0f)));
        
        // Add shape to scene
        Scene3DNode *rootNode = Scene3D_getRootNode(&_scene);
        Scene3DNode_setRenderStyle(rootNode, kRenderFilled);
        Scene3DNode_addShape(rootNode, &_shape, Vector3DMake(0.0f, 0.0f, 0.0f));
    }
    
    ~Icosohedron() {
        // Note: This crashes :(
        //Scene3D_deinit(&_scene);
    }
    
    void Update() override {
        // Clear screen
        Graphics::Clear(kColorWhite);

        // Rotate scene
        _rotation += 0.02f;
        
        // Apply rotation to scene transform
        Scene3DNode *rootNode = Scene3D_getRootNode(&_scene);
        Matrix3D rotationMatrix = CreateRotationMatrix(_rotation);
        Scene3DNode_setTransform(rootNode, &rotationMatrix);
        
        // Draw
        Scene3D_draw(&_scene, Graphics::GetFrame(), LCD_ROWSIZE);
        
        // Mark all rows ready to update
        Graphics::MarkUpdatedRows(0, LCD_ROWS-1);
    }
    
    Matrix3D CreateRotationMatrix(float rotation) {
        return Matrix3DMake( cosf(rotation),            0.0f,  sinf(rotation),
                                       0.0f,            1.0f,            0.0f,
                            -sinf(rotation),            0.0f,  cosf(rotation));
    }
private:
    Scene3D _scene;
    Shape3D _shape;
    float   _rotation;
};

