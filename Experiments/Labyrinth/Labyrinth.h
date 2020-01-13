//
//  Labyrinth.h
//  PlaydateNative Simulator
//
//  Created by Max Weisel on 11/12/19.
//  Copyright © 2019 Max Weisel. All rights reserved.
//

#pragma once

#include "../../Playdate/Playdate.h"

#include "PlaydateHD.h"

using namespace Playdate;

/*
extern "C" {
    void (*startAccelerometer)(void) = (void (*)(void))0x08014060;
    void (*readAccelerometer)(float* outX, float* outY, float* outZ) = (void (*)(float* outX, float* outY, float* outZ))0x08014140;
}
*/

class Labyrinth : public Application {
public:
    Labyrinth() {
        // Set up
        _ballPosition = Display::Size() / 2.0f;
        
        // Start accelerometer
        System::SetPeripheralsEnabled(kAllPeripherals);
    }
    
    ~Labyrinth() {
        // Tear down
        
    }
    
    void Update() override {
        // Update
        
        // Clear screen
        //Graphics::Clear(kColorWhite);
        _playdateHD.Render();
        
        // Draw FPS indicator
        System::DrawFPS(10, 10);
        
        Vector2f acceleration = System::Accelerometer();
        
        // Update ball position
        float mass = 1.0f;
        _ballVelocity += acceleration / mass;
        
        Vector2i min = Vector2i::zero;
        Vector2i max = Display::Size();
        
        PhysicsStep(&_ballPosition.x, &_ballVelocity.x, 10.0f, min.x, max.x);
        PhysicsStep(&_ballPosition.y, &_ballVelocity.y, 10.0f, min.y, max.y);

        // Draw ball
        //DrawBall();
    }
    
    static void PhysicsStep(float *position, float *velocity, const float radius, int min, int max) {
        float newPosition = *position + *velocity;
        
        // Min edge bounce
        if (newPosition - radius < min) {
            // Bounce back in bounds
            float distanceOutOfBounds = min - (newPosition - radius);
            newPosition = min + distanceOutOfBounds + radius;
        
            // Flip velocity
            *velocity *= -0.8f;
        }
        
        // Max edge bounce
        if (newPosition + radius > max) {
            // Bounce back in bounds
            float distanceOutOfBounds = (newPosition + radius) - max;
            newPosition = max - distanceOutOfBounds - radius;
        
            // Flip velocity
            *velocity *= -0.8f;
        }
        
        // Friction
        *velocity *= 0.998f;
        
        *position = newPosition;
    }
    
    void DrawBall() {
        const float radius = 10.0f;
        float x      = _ballPosition.x - radius;
        float y      = _ballPosition.y - radius;
        float width  = radius * 2.0f;
        float height = radius * 2.0f;
        Graphics::DrawEllipse(x, y, width, height, 0, 360);
    }
private:
    Vector2f _ballPosition;
    Vector2f _ballVelocity;
    
    PlaydateHD _playdateHD;
};


