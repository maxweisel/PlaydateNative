//
//  CrankDrawing.h
//  PlaydateNative
//
//  Created by Max Weisel on 12/8/19.
//  Copyright © 2019 Max Weisel. All rights reserved.
//

#pragma once

#include "../../Playdate/Playdate.h"
#include <math.h>

#define M_PI 3.14159265358979323846264338327950288 /* pi             */

using namespace Playdate;

class CrankDrawing : public Application {
public:
    CrankDrawing() {
        Graphics::Clear(kColorWhite);
        
        _cursor = Display::Size() / 2.0f;
    }
    
    ~CrankDrawing() {
        // Tear down
        
    }
    
    void Update() override {
        // Update
        
        // Clear screen
        //Graphics::Clear(kColorWhite);
        
        // Get buttons
        PDButtons current, pressed, released;
        System::ButtonState(&current, &pressed, &released);
        
        if ((current & kButtonUp) == kButtonUp) {
            // Move forward
            
            Vector2f start = _cursor;
            Vector2f end   = _cursor;
            
            // Move cursor
            float crankAngle = (System::CrankAngle()-90.0f) / 180.0f * M_PI;
            float x = cosf(crankAngle) * 2.0f;
            float y = sinf(crankAngle) * 2.0f;
            end.x += x;
            end.y += y;
            _cursor = end;
            
            // Shade
            bool xDirection = end.x >= start.x;
            bool yDirection = end.y >= start.y;
            start.x = xDirection ? floorf(start.x) :  ceilf(start.x);
            start.y = yDirection ? floorf(start.y) :  ceilf(start.y);
              end.x = xDirection ?  ceilf(  end.x) : floorf(  end.x);
              end.y = yDirection ?  ceilf(  end.y) : floorf(  end.y);
            
            Graphics::DrawLine((int)start.x, (int)start.y, (int)end.x, (int)end.y, 4);
            
            float yMin = start.y < end.y ? start.y : end.y;
            float yMax = start.y > end.y ? start.y : end.y;
            Graphics::MarkUpdatedRows((int)yMin, (int)yMax);
        }
    }
private:
    Vector2f _cursor;
};


