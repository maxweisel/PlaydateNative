//
//  Labyrinth.hpp
//  PlaydateNative Simulator
//
//  Created by Max Weisel on 11/12/19.
//  Copyright © 2019 Max Weisel. All rights reserved.
//

#pragma once

#include "../../Playdate/Playdate.h"

using namespace Playdate;

extern "C" {
    //void (*startAccelerometer)(void) = (void (*)(void))0x8014130;
    void (*readAccelerometer)(float* outX, float* outY, float* outZ) = (void (*)(float* outX, float* outY, float* outZ))0x8014210;
}

class Labyrinth : public Application {
public:
    Labyrinth() {
        // Set up
        _ballX = Display::Width()  / 2;
        _ballY = Display::Height() / 2;
        
        //startAccelerometer();
    }
    
    ~Labyrinth() {
        // Tear down
        
    }
    
    void Update() override {
        // Update
        
        // Clear screen
        Graphics::Clear(kColorWhite);
        
        // Draw FPS indicator
        System::DrawFPS(10, 10);
        
        float x, y, z;
        readAccelerometer(&x, &y, &z);
        
        // Update ball position
        //_ballVelocityX += x;
        //_ballVelocityY += y;
        //_ballX += _ballVelocityX;
        //_ballY += _ballVelocityY;
        _ballX = x;
        _ballY = y;
        
        // Draw ball
        DrawBall();
    }
    
    void DrawBall() {
        const float radius = 10.0f;
        float x      = _ballX - radius;
        float y      = _ballY - radius;
        float width  = radius * 2.0f;
        float height = radius * 2.0f;
        Graphics::DrawEllipse(x, y, width, height, 0, 360);
    }
private:
    float _ballX;
    float _ballY;
    float _ballVelocityX;
    float _ballVelocityY;
};


