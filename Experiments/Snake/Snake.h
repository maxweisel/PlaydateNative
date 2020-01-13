//
//  CrankDrawing.h
//  PlaydateNative
//
//  Created by Max Weisel on 1/6/19.
//  Copyright © 2020 Max Weisel. All rights reserved.
//

#pragma once

#include "../../Playdate/Playdate.h"
#include <math.h>
#include <vector>

#define M_PI 3.14159265358979323846264338327950288 /* pi             */

using namespace Playdate;

class Snake : public Application {
public:
    Snake() {
        Graphics::Clear(kColorWhite);
        
        _cursor = Display::Size() / 2.0f;
    }
    
    ~Snake() {
        // Tear down
        
    }
    
    void Update() override {
        // Update
        
        // Clear screen
        Graphics::Clear(kColorWhite);
        
        // Draw FPS indicator
        System::DrawFPS(10, 10);
        
        // Move cursor
        float crankAngle = (System::CrankAngle()-90.0f) / 180.0f * M_PI;
        _cursor.x += cosf(crankAngle) * 2.0f;
        _cursor.y += sinf(crankAngle) * 2.0f;
        
        // Add new point to vector
        _snakePoints.push_back(_cursor);
        
        // Draw
        DrawSnake();
    }
    
    void DrawSnake() {
        for (int i = 0; i < _snakePoints.size()-1; i++) {
            Vector2f start = _snakePoints[i];
            Vector2f end = _snakePoints[i+1];
            
            bool xDirection = end.x >= start.x;
            bool yDirection = end.y >= start.y;
            start.x = xDirection ? floorf(start.x) :  ceilf(start.x);
            start.y = yDirection ? floorf(start.y) :  ceilf(start.y);
              end.x = xDirection ?  ceilf(  end.x) : floorf(  end.x);
              end.y = yDirection ?  ceilf(  end.y) : floorf(  end.y);
            
            Graphics::DrawLine((int)start.x, (int)start.y, (int)end.x, (int)end.y, 8);
            
            // Not needed because Graphics::Clear marks all rows as updated under the hood.
            //float yMin = start.y < end.y ? start.y : end.y;
            //float yMax = start.y > end.y ? start.y : end.y;
            //Graphics::MarkUpdatedRows((int)yMin, (int)yMax);
        }
    }
private:
    Vector2f _cursor;
    
    std::vector<Vector2f> _snakePoints;
};


