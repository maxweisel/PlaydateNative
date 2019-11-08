//
//  SampleApplication.h
//  PlaydateNative
//
//  Created by Max Weisel on 10/20/19.
//  Copyright © 2019 Max Weisel. All rights reserved.
//

#pragma once

#include "Playdate.h"

using namespace Playdate;

class SampleApplication : public Application {
public:
    SampleApplication() {
        // Set up
        
    }
    
    ~SampleApplication() {
        // Tear down
        
    }
    
    void Update() override {
        // Update
        
        // Clear screen
        Graphics::Clear(kColorWhite);
        
        // Draw FPS indicator
        System::DrawFPS(10, 10);
        
    }
private:
    
};


